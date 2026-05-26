from __future__ import annotations

import argparse
import io
import shutil
import struct
import sys
from collections.abc import Iterator
from dataclasses import dataclass
from pathlib import Path
from typing import Final

try:
    from PIL import Image
except ImportError:  # pragma: no cover - runtime dependency check
    Image = None


Hotspot = tuple[int, int]
IconSize = tuple[int, int]

ICON_HEADER_SIZE: Final = 6
ICON_DIR_ENTRY_SIZE: Final = 16
SINGLE_ENTRY_IMAGE_OFFSET: Final = ICON_HEADER_SIZE + ICON_DIR_ENTRY_SIZE
RIFF_HEADER_SIZE: Final = 12
RIFF_CHUNK_HEADER_SIZE: Final = 8
ANI_HEADER_SIZE: Final = 36
COLUMN_STARTS: Final = (0, 20, 40)
SUPPORTED_EXTENSIONS: Final = {".cur", ".ani"}
CURSOR_ICON_TYPE: Final = 2
LIST_CHUNK_ID: Final = b"LIST"
FRAM_LIST_TYPE: Final = b"fram"
ICON_CHUNK_ID: Final = b"icon"


@dataclass(frozen=True)
class IconFrame:
    """A single image entry stored inside a CUR/ICO container."""

    width: int
    height: int
    hotspot: Hotspot | None
    icon_type: int
    dir_entry: bytes
    image_data: bytes


@dataclass(frozen=True)
class AniAnimation:
    """ANI playback data after resolving frame order and timing."""

    steps: list[bytes]
    frame_times: list[int]


@dataclass(frozen=True)
class RiffChunk:
    """A RIFF chunk identifier plus its raw payload."""

    chunk_id: bytes
    payload: bytes


def ensure_pillow_available() -> None:
    """Fail fast with a clear message if Pillow is missing."""

    if Image is None:
        raise RuntimeError("Pillow is required. Please run: python -m pip install pillow")


def parse_icon_size(value: str) -> IconSize:
    """Parse the CLI size argument written as N or WxH."""

    text = value.strip().lower()
    try:
        if "x" in text:
            width_text, height_text = text.split("x", 1)
            width = int(width_text)
            height = int(height_text)
        else:
            width = height = int(text)
    except ValueError as exc:
        raise argparse.ArgumentTypeError(
            "Icon size must be an integer or WxH, for example 32 or 32x32"
        ) from exc

    if width <= 0 or height <= 0:
        raise argparse.ArgumentTypeError("Icon size must be greater than 0")

    return width, height


def iter_cursor_files(input_root: Path) -> list[Path]:
    """Return all CUR and ANI files under the input folder in stable order."""

    return sorted(
        path
        for path in input_root.rglob("*")
        if path.is_file() and path.suffix.lower() in SUPPORTED_EXTENSIONS
    )


def iter_riff_chunks(
    data: bytes,
    *,
    start: int = 0,
    end: int | None = None,
    context: str,
) -> Iterator[RiffChunk]:
    """Yield RIFF chunks while centralizing bounds checks in one place."""

    chunk_end = len(data) if end is None else end
    position = start

    while position + RIFF_CHUNK_HEADER_SIZE <= chunk_end:
        chunk_id = data[position : position + 4]
        chunk_size = struct.unpack_from("<I", data, position + 4)[0]
        payload_start = position + RIFF_CHUNK_HEADER_SIZE
        payload_end = payload_start + chunk_size

        if payload_end > chunk_end:
            raise ValueError(f"{context} chunk exceeds bounds")

        yield RiffChunk(chunk_id=chunk_id, payload=data[payload_start:payload_end])
        position = payload_end + (chunk_size & 1)


def parse_u32_array(payload: bytes, chunk_name: str) -> list[int]:
    """Parse a RIFF payload made of little-endian uint32 values."""

    if len(payload) % 4 != 0:
        raise ValueError(f"Invalid {chunk_name} chunk format")

    count = len(payload) // 4
    if count == 0:
        return []

    return list(struct.unpack(f"<{count}I", payload))


def parse_ani_header(payload: bytes) -> int:
    """Read the ANI header and return the default jiffies delay."""

    if len(payload) < ANI_HEADER_SIZE:
        raise ValueError("anih chunk is too short")

    fields = struct.unpack_from("<9I", payload)
    return fields[7]


def extract_fram_icons(list_payload: bytes) -> list[bytes]:
    """Extract each embedded icon frame from a LIST/fram chunk."""

    if len(list_payload) < 4:
        raise ValueError("LIST chunk is too short")

    if list_payload[:4] != FRAM_LIST_TYPE:
        return []

    return [
        chunk.payload
        for chunk in iter_riff_chunks(list_payload, start=4, context="fram")
        if chunk.chunk_id == ICON_CHUNK_ID
    ]


def resolve_animation_steps(icon_chunks: list[bytes], step_indices: list[int]) -> list[bytes]:
    """Resolve the ANI playback order from the optional seq chunk."""

    if not step_indices:
        raise ValueError("ANI contains no playback frames")

    steps: list[bytes] = []
    for step_index in step_indices:
        if step_index >= len(icon_chunks):
            raise ValueError("ANI seq index is out of frame range")
        steps.append(icon_chunks[step_index])

    return steps


def build_frame_times(default_rate: int, rate_values: list[int], step_count: int) -> list[int]:
    """Build one delay value per playback step."""

    if not rate_values:
        return [default_rate] * step_count

    frame_times = rate_values[:step_count]
    if len(frame_times) < step_count:
        frame_times.extend([default_rate] * (step_count - len(frame_times)))

    return frame_times


def parse_icon_entries(icon_bytes: bytes) -> list[IconFrame]:
    """Parse all directory entries from a CUR or ICO container."""

    if len(icon_bytes) < ICON_HEADER_SIZE:
        raise ValueError("Icon file is too small")

    reserved, icon_type, count = struct.unpack_from("<HHH", icon_bytes, 0)
    if reserved != 0 or icon_type not in (1, CURSOR_ICON_TYPE):
        raise ValueError("Invalid CUR/ICO data")

    directory_end = ICON_HEADER_SIZE + count * ICON_DIR_ENTRY_SIZE
    if len(icon_bytes) < directory_end:
        raise ValueError("Icon directory is incomplete")

    frames: list[IconFrame] = []
    for index in range(count):
        entry_offset = ICON_HEADER_SIZE + index * ICON_DIR_ENTRY_SIZE
        dir_entry = bytes(icon_bytes[entry_offset : entry_offset + ICON_DIR_ENTRY_SIZE])
        width = dir_entry[0] or 256
        height = dir_entry[1] or 256
        image_size, image_offset = struct.unpack_from("<II", dir_entry, 8)
        image_end = image_offset + image_size

        if image_end > len(icon_bytes):
            raise ValueError("Icon directory entry exceeds bounds")

        hotspot = struct.unpack_from("<HH", dir_entry, 4) if icon_type == CURSOR_ICON_TYPE else None
        frames.append(
            IconFrame(
                width=width,
                height=height,
                hotspot=hotspot,
                icon_type=icon_type,
                dir_entry=dir_entry,
                image_data=icon_bytes[image_offset:image_end],
            )
        )

    return frames


def select_icon_frame(icon_bytes: bytes, target_size: IconSize) -> IconFrame | None:
    """Return the icon entry that exactly matches the requested size."""

    target_width, target_height = target_size
    for frame in parse_icon_entries(icon_bytes):
        if frame.width == target_width and frame.height == target_height:
            return frame
    return None


def build_single_entry_icon(frame: IconFrame) -> bytes:
    """Wrap the chosen image in a one-entry CUR/ICO container for Pillow."""

    header = struct.pack("<HHH", 0, frame.icon_type, 1)
    dir_entry = bytearray(frame.dir_entry)

    # Pillow reliably decodes the requested size once it is the only directory entry.
    struct.pack_into("<I", dir_entry, 12, SINGLE_ENTRY_IMAGE_OFFSET)
    return header + bytes(dir_entry) + frame.image_data


def save_icon_frame(frame: IconFrame, output_path: Path) -> None:
    """Decode a selected cursor frame and persist it as a PNG file."""

    ensure_pillow_available()
    assert Image is not None

    output_path.parent.mkdir(parents=True, exist_ok=True)
    payload = build_single_entry_icon(frame)

    with Image.open(io.BytesIO(payload)) as image:
        image.convert("RGBA").save(output_path)


def parse_ani_file(ani_bytes: bytes) -> AniAnimation:
    """Parse an ANI file into playback steps and frame delays."""

    if len(ani_bytes) < RIFF_HEADER_SIZE or ani_bytes[:4] != b"RIFF" or ani_bytes[8:12] != b"ACON":
        raise ValueError("Invalid ANI file")

    default_rate = 0
    rate_values: list[int] = []
    seq_values: list[int] = []
    icon_chunks: list[bytes] = []

    for chunk in iter_riff_chunks(ani_bytes, start=RIFF_HEADER_SIZE, context="ANI"):
        if chunk.chunk_id == b"anih":
            default_rate = parse_ani_header(chunk.payload)
        elif chunk.chunk_id == b"rate":
            rate_values = parse_u32_array(chunk.payload, "rate")
        elif chunk.chunk_id == b"seq ":
            seq_values = parse_u32_array(chunk.payload, "seq")
        elif chunk.chunk_id == LIST_CHUNK_ID:
            icon_chunks.extend(extract_fram_icons(chunk.payload))

    if not icon_chunks:
        raise ValueError("ANI contains no exportable icon frames")

    step_indices = seq_values or list(range(len(icon_chunks)))
    steps = resolve_animation_steps(icon_chunks, step_indices)
    frame_times = build_frame_times(default_rate, rate_values, len(steps))
    return AniAnimation(steps=steps, frame_times=frame_times)


def format_hotspot(hotspot: Hotspot) -> str:
    """Format one hotspot pair using the float text requested by the caller."""

    x, y = hotspot
    return f"{x:.1f}f, {y:.1f}f"


def format_hotspots(hotspots: list[Hotspot]) -> str:
    """Collapse hotspot values to one field, preserving per-frame changes when needed."""

    if not hotspots:
        return format_hotspot((0, 0))

    if len(set(hotspots)) == 1:
        return format_hotspot(hotspots[0])

    return "|".join(format_hotspot(hotspot) for hotspot in hotspots)


def format_frame_time(jiffies: int) -> str:
    """Format one frame delay using exact jiffies over 60 seconds."""

    return f"{jiffies}s/60"


def format_frame_times(frame_times: list[int]) -> str:
    """Collapse frame delays to one field, preserving per-frame changes when needed."""

    if not frame_times:
        return format_frame_time(0)

    if len(set(frame_times)) == 1:
        return format_frame_time(frame_times[0])

    return "|".join(format_frame_time(frame_time) for frame_time in frame_times)


def pad_to_column(parts: list[str], current_length: int, target_start: int) -> int:
    """Pad with spaces until the target column, or at least one space after overflow."""

    if current_length < target_start:
        padding = target_start - current_length
    elif current_length > target_start and parts:
        padding = 1
    else:
        padding = 0

    if padding:
        parts.append(" " * padding)
        current_length += padding

    return current_length


def format_property_row(*columns: str) -> str:
    """Lay out columns so they aim for the visual starts 0, 20 and 40."""

    parts: list[str] = []
    current_length = 0

    for index, column in enumerate(columns):
        target_start = COLUMN_STARTS[index] if index < len(COLUMN_STARTS) else current_length
        current_length = pad_to_column(parts, current_length, target_start)
        parts.append(column)
        current_length += len(column)

    return "".join(parts)


def relative_cursor_name(input_root: Path, cursor_path: Path) -> str:
    """Build the property-file name using a stable relative POSIX path."""

    return cursor_path.relative_to(input_root).with_suffix("").as_posix()


def build_cur_property_row(input_root: Path, cursor_path: Path, hotspot: Hotspot) -> str:
    """Build the frame_prop row for a static CUR file."""

    return format_property_row(
        relative_cursor_name(input_root, cursor_path),
        format_hotspot(hotspot),
    )


def build_ani_property_row(
    input_root: Path,
    cursor_path: Path,
    hotspots: list[Hotspot],
    frame_times: list[int],
) -> str:
    """Build the frame_prop row for an ANI file."""

    return format_property_row(
        relative_cursor_name(input_root, cursor_path),
        format_hotspots(hotspots),
        format_frame_times(frame_times),
    )


def print_warning(message: str) -> None:
    """Emit user-facing skip or parse warnings to stderr."""

    print(f"Warning: {message}", file=sys.stderr)


def warn_missing_size(cursor_path: Path, target_size: IconSize) -> None:
    """Explain that a file does not contain the requested cursor size."""

    print_warning(f"{cursor_path} does not contain a {target_size[0]}x{target_size[1]} icon. Skipped.")


def reset_output_dir(output_dir: Path) -> None:
    """Clear an animation output folder before writing fresh frame PNG files."""

    if output_dir.exists():
        shutil.rmtree(output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)


def select_animation_frames(
    animation: AniAnimation,
    target_size: IconSize,
    cursor_path: Path,
) -> list[IconFrame] | None:
    """Pick the requested-size image for every ANI playback step."""

    frames: list[IconFrame] = []
    for step_bytes in animation.steps:
        try:
            frame = select_icon_frame(step_bytes, target_size)
        except Exception as exc:
            print_warning(f"Failed to parse animation frame in {cursor_path}: {exc}")
            return None

        if frame is None:
            warn_missing_size(cursor_path, target_size)
            return None

        frames.append(frame)

    return frames


def export_animation_frames(output_dir: Path, frames: list[IconFrame], cursor_path: Path) -> list[Hotspot] | None:
    """Write all ANI playback frames and return their hotspot list."""

    reset_output_dir(output_dir)
    hotspots: list[Hotspot] = []

    for index, frame in enumerate(frames, start=1):
        try:
            save_icon_frame(frame, output_dir / f"{index}.png")
        except Exception as exc:
            print_warning(f"Failed to export frame {index} from {cursor_path}: {exc}")
            return None
        hotspots.append(frame.hotspot or (0, 0))

    return hotspots


def process_cur_file(
    input_root: Path,
    output_root: Path,
    cursor_path: Path,
    target_size: IconSize,
    properties: list[str],
) -> bool:
    """Export one CUR file and append its property row on success."""

    try:
        frame = select_icon_frame(cursor_path.read_bytes(), target_size)
    except Exception as exc:
        print_warning(f"Failed to parse {cursor_path}: {exc}")
        return False

    if frame is None:
        warn_missing_size(cursor_path, target_size)
        return False

    output_path = output_root / cursor_path.relative_to(input_root).with_suffix(".png")
    try:
        save_icon_frame(frame, output_path)
    except Exception as exc:
        print_warning(f"Failed to export {cursor_path}: {exc}")
        return False

    properties.append(build_cur_property_row(input_root, cursor_path, frame.hotspot or (0, 0)))
    return True


def process_ani_file(
    input_root: Path,
    output_root: Path,
    cursor_path: Path,
    target_size: IconSize,
    properties: list[str],
) -> bool:
    """Export one ANI file and append its property row on success."""

    try:
        animation = parse_ani_file(cursor_path.read_bytes())
    except Exception as exc:
        print_warning(f"Failed to parse {cursor_path}: {exc}")
        return False

    frames = select_animation_frames(animation, target_size, cursor_path)
    if frames is None:
        return False

    output_dir = output_root / cursor_path.relative_to(input_root).with_suffix("")
    hotspots = export_animation_frames(output_dir, frames, cursor_path)
    if hotspots is None:
        return False

    properties.append(build_ani_property_row(input_root, cursor_path, hotspots, animation.frame_times))
    return True


def process_cursor_directory(
    input_root: Path,
    output_root: Path,
    target_size: IconSize,
) -> tuple[list[str], int, int, int]:
    """Process every supported cursor file under the input directory."""

    properties: list[str] = []
    cur_count = 0
    ani_count = 0
    skipped_count = 0

    for cursor_path in iter_cursor_files(input_root):
        if cursor_path.suffix.lower() == ".cur":
            success = process_cur_file(input_root, output_root, cursor_path, target_size, properties)
            cur_count += int(success)
        else:
            success = process_ani_file(input_root, output_root, cursor_path, target_size, properties)
            ani_count += int(success)

        if not success:
            skipped_count += 1

    return properties, cur_count, ani_count, skipped_count


def write_property_file(output_root: Path, properties: list[str]) -> Path:
    """Persist frame_prop.txt and return its final path."""

    frame_prop_path = output_root / "frame_prop.txt"
    frame_prop_path.write_text("\n".join(properties) + ("\n" if properties else ""), encoding="utf-8")
    return frame_prop_path


def build_argument_parser() -> argparse.ArgumentParser:
    """Create the command-line interface for the extractor script."""

    parser = argparse.ArgumentParser(
        description=(
            "Export CUR and ANI files from an input directory to PNG files at the requested size, "
            "and write hotspot and frame properties."
        )
    )
    parser.add_argument("input_path", type=Path, help="Input directory path")
    parser.add_argument("output_path", type=Path, help="Output directory path")
    parser.add_argument("icon_size", type=parse_icon_size, help="Target icon size, for example 32 or 32x32")
    return parser


def main(argv: list[str] | None = None) -> int:
    """Run the cursor extraction CLI."""

    parser = build_argument_parser()
    args = parser.parse_args(argv)

    try:
        ensure_pillow_available()
    except RuntimeError as exc:
        print(exc, file=sys.stderr)
        return 1

    input_root = args.input_path.expanduser().resolve()
    output_root = args.output_path.expanduser().resolve()
    target_size = args.icon_size

    if not input_root.exists() or not input_root.is_dir():
        parser.error("Input path must be an existing directory")

    output_root.mkdir(parents=True, exist_ok=True)
    properties, cur_count, ani_count, skipped_count = process_cursor_directory(
        input_root,
        output_root,
        target_size,
    )
    frame_prop_path = write_property_file(output_root, properties)

    print()
    print(
        f"Done: exported {cur_count} CUR files, {ani_count} ANI files, "
        f"skipped {skipped_count} files."
    )
    print(f"Property file written to: {frame_prop_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
