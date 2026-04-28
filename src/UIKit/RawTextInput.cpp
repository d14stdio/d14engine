#include "Common/Precompile.h"

#include "UIKit/RawTextInput.h"

#include "Common/DirectXError.h"

#include "UIKit/Application.h"
#include "UIKit/PlatformUtils.h"
#include "UIKit/ResourceUtils.h"

using namespace d14engine::renderer;

namespace d14engine::uikit
{
    RawTextInput::RawTextInput(
        bool multiline,
        float roundRadius,
        const D2D1_RECT_F& rect)
        :
        LabelArea({}, rect),
        multiline(multiline)
    {
        roundRadiusX = roundRadiusY = roundRadius;

        m_placeholder = makeUIObject<Label>();
    }

    void RawTextInput::initialize()
    {
        LabelArea::initialize();

        registerUIEvents(m_placeholder);

        if (multiline)
        {
            THROW_IF_FAILED(m_textLayout->
                SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));

            THROW_IF_FAILED(m_placeholder->textLayout()->
                SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));
        }
        setVisibleTextRect(selfCoordRect());
    }

    void RawTextInput::onTextContentOffsetChange(const D2D1_POINT_2F& offset)
    {
        onTextContentOffsetChangeHelper(offset);

        if (f_onTextContentOffsetChange) f_onTextContentOffsetChange(this, offset);
    }

    void RawTextInput::onTextContentOffsetChangeHelper(const D2D1_POINT_2F& offset)
    {
        // This method intentionally left blank.
    }

    const D2D1_RECT_F& RawTextInput::visibleTextRect() const
    {
        return m_visibleTextRect;
    }

    void RawTextInput::setVisibleTextRect(const D2D1_RECT_F& rect)
    {
        m_visibleTextRect = rect;

        float maskWidth = math_utils::width(m_visibleTextRect);
        float maskHeight = math_utils::height(m_visibleTextRect);

        THROW_IF_FAILED(m_textLayout->SetMaxWidth(maskWidth));
        THROW_IF_FAILED(m_textLayout->SetMaxHeight(maskHeight));

        setCaretPosition(m_caretPosition);
        setSelectedRange(m_selectedRange);

        m_visibleTextMask.loadBitmap(maskWidth, maskHeight);

        m_placeholder->transform(m_visibleTextRect);
    }

    D2D1_POINT_2F RawTextInput::validateTextContentOffset(const D2D1_POINT_2F& in)
    {
        D2D1_POINT_2F out = { 0.0f, 0.0f };

        auto metrics = textMetrics();
        auto metrics_width = metrics.widthIncludingTrailingWhitespace;

        out.x = std::clamp(in.x, 0.0f, std::max(metrics_width  - metrics.layoutWidth,  0.0f));
        out.y = std::clamp(in.y, 0.0f, std::max(metrics.height - metrics.layoutHeight, 0.0f));

        return out;
    }

    const D2D1_POINT_2F& RawTextInput::textContentOffset() const
    {
        return m_textContentOffset;
    }

    void RawTextInput::setTextContentOffset(const D2D1_POINT_2F& offset)
    {
        auto validOffset = validateTextContentOffset(offset);

        if (validOffset.x != m_textContentOffset.x ||
            validOffset.y != m_textContentOffset.y)
        {
            m_textContentOffset = validOffset;
            onTextContentOffsetChange(validOffset);
        }
    }

    void RawTextInput::setTextContentOffsetSilently(const D2D1_POINT_2F& offset)
    {
        m_textContentOffset = validateTextContentOffset(offset);
    }

    const SharedPtr<Label>& RawTextInput::placeholder() const
    {
        return m_placeholder;
    }

    void RawTextInput::editSelectedText(WstrRefer text)
    {
        if (setSelectedTextHelper(text))
        {
            onTextChanged(m_text);
            onTextEdited(m_text);
        }
    }

    void RawTextInput::performCommandCutSelection()
    {
        auto selectedText = m_text.substr
        (
        /* _Off   */ m_selectedRange.offset,
        /* _Count */ m_selectedRange.count
        );
        resource_utils::setClipboardText(selectedText);

        if (eraseTextHelper(m_selectedRange))
        {
            setCaretPosition(m_selectedRange.offset);
            setSelectedRange({ 0, 0 });

            onTextChanged(m_text);
            onTextEdited(m_text);
        }
    }

    void RawTextInput::performCommandPasteSelection()
    {
        auto content = resource_utils::getClipboardText();
        if (content.has_value())
        {
            if (setSelectedTextHelper(content.value()))
            {
                onTextChanged(m_text);
                onTextEdited(m_text);
            }
        }
    }

    Optional<LOGFONT> RawTextInput::getCompositionFont() const
    {
        LOGFONT font =
        {
            .lfHeight = platform_utils::scaledByDpi<LONG>(
            (
                m_caretGeometry.second.y - m_caretGeometry.first.y
            )),
            .lfWidth          = 0, // keep the aspect ratio
            .lfEscapement     = 0,
            .lfOrientation    = 0,
            .lfWeight         = m_textLayout->GetFontWeight(),
            .lfItalic         = FALSE,
            .lfUnderline      = FALSE,
            .lfStrikeOut      = FALSE,
            .lfCharSet        = DEFAULT_CHARSET,
            .lfOutPrecision   = OUT_DEFAULT_PRECIS,
            .lfClipPrecision  = CLIP_DEFAULT_PRECIS,
            .lfQuality        = CLEARTYPE_NATURAL_QUALITY,
            .lfPitchAndFamily = DEFAULT_PITCH
        };
        THROW_IF_FAILED(m_textLayout->GetFontFamilyName
        (
            font.lfFaceName, _countof(font.lfFaceName)
        ));
        return font;
    }

    Optional<COMPOSITIONFORM> RawTextInput::getCompositionForm() const
    {
        auto origin = selfCoordToAbsolute(math_utils::leftTop(m_visibleTextRect));
        COMPOSITIONFORM form =
        {
            .dwStyle = CFS_POINT,
            .ptCurrentPos = math_utils::roundl(platform_utils::scaledByDpi(D2D1_POINT_2F
            {
                origin.x + m_caretGeometry.first.x - m_textContentOffset.x,
                origin.y + m_caretGeometry.first.y - m_textContentOffset.y
            }))
        };
        return form;
    }

    void RawTextInput::setText(WstrRefer text)
    {
        if (setTextHelper(text))
        {
            setCaretPosition(0);
            setSelectedRange({ 0, 0 });

            onTextChanged(m_text);
        }
    }

    void RawTextInput::setCaretPosition(size_t position)
    {
        LabelArea::setCaretPosition(position);

        // Make a copy here as we need to modify it.
        auto offset = m_textContentOffset;

        auto width = math_utils::width(m_visibleTextRect);
        auto height = math_utils::height(m_visibleTextRect);

        if (m_caretGeometry.first.x < offset.x)
        {
            offset.x = m_caretGeometry.first.x;
        }
        else if (m_caretGeometry.second.x > offset.x + width)
        {
            offset.x = m_caretGeometry.second.x - width;
        }
        if (m_caretGeometry.first.y < offset.y)
        {
            offset.y = m_caretGeometry.first.y;
        }
        else if (m_caretGeometry.second.y > offset.y + height)
        {
            offset.y = m_caretGeometry.second.y - height;
        }
        setTextContentOffset(offset);
    }

    void RawTextInput::setSelectedText(WstrRefer text)
    {
        if (setSelectedTextHelper(text))
        {
            onTextChanged(m_text);
        }
    }

    void RawTextInput::onRendererDrawD2d1LayerHelper(Renderer* rndr)
    {
        //------------------------------------------------------------------
        // Note the difference between these two text anti-aliasing modes:
        //------------------------------------------------------------------
        // 1. Grayscale:
        //    The rendering result is independent of the target background,
        //    so opacity can be set as needed (any value from 0 ~ 1 is OK).
        //
        // 2. ClearType:
        //    The rendering result depends on the target background color,
        //    so you must set an opaque background (better a value >= 0.5).
        //
        //------------------------------------------------------------------
        // Set alpha channel carefully so that text can display correctly.
        //------------------------------------------------------------------
        m_visibleTextMask.color = Label::appearance().background.color;
        m_visibleTextMask.color.a = Label::appearance().background.opacity;

        m_visibleTextMask.beginDraw(rndr->d2d1DeviceContext());
        {
            // Placeholder
            if (m_placeholder->isD2d1ObjectVisible() && m_text.empty())
            {
                auto placeholderTrans = D2D1::Matrix3x2F::Translation
                (
                    -m_placeholder->absoluteX(), -m_placeholder->absoluteY()
                );
                rndr->d2d1DeviceContext()->SetTransform(placeholderTrans);

                m_placeholder->onRendererDrawD2d1Object(rndr);
            }
            // Text Content
            auto textContentTrans = D2D1::Matrix3x2F::Translation
            (
                - (m_absoluteRect.left + m_textContentOffset.x),
                - (m_absoluteRect.top  + m_textContentOffset.y)
            );
            rndr->d2d1DeviceContext()->SetTransform(textContentTrans);

            // The caret will be drawn above the visible text mask.
            drawSelection(rndr); drawTextLayout(rndr); /* drawCaret(rndr); */
        }
        m_visibleTextMask.endDraw(rndr->d2d1DeviceContext());
    }

    void RawTextInput::onRendererDrawD2d1ObjectHelper(Renderer* rndr)
    {
        ////////////////
        // Background //
        ////////////////

        drawBackground(rndr);

        //////////////////
        // Visible Text //
        //////////////////

        auto dstRect = math_utils::roundf(selfCoordToAbsolute(m_visibleTextRect));

        rndr->d2d1DeviceContext()->DrawBitmap
        (
        /* bitmap               */ m_visibleTextMask.data.Get(),
        /* destinationRectangle */ dstRect,
        /* opacity              */ m_visibleTextMask.opacity,
        /* interpolationMode    */ m_visibleTextMask.getInterpolationMode()
        );

        //////////////
        // Caret(|) //
        //////////////

        D2D1_MATRIX_3X2_F originalTrans = {};
        rndr->d2d1DeviceContext()->GetTransform(&originalTrans);

        auto indicatorTrans = D2D1::Matrix3x2F::Translation
        (
            std::round(m_visibleTextRect.left - m_textContentOffset.x),
            std::round(m_visibleTextRect.top  - m_textContentOffset.y)
        );
        rndr->d2d1DeviceContext()->SetTransform(originalTrans * indicatorTrans);
        {
            caretConstrainedRect = math_utils::rect
            (
                m_textContentOffset, math_utils::size(m_visibleTextRect)
            );
            drawCaret(rndr); // Vertical Line Caret
        }
        rndr->d2d1DeviceContext()->SetTransform(originalTrans);

        /////////////
        // Outline //
        /////////////

        drawOutline(rndr);

        /////////////////
        // Bottom Line //
        /////////////////

        auto& bottomLineBkgn = appearance().bottomLine.background;

        resource_utils::solidColorBrush()->SetColor(bottomLineBkgn.color);
        resource_utils::solidColorBrush()->SetOpacity(bottomLineBkgn.opacity);

        auto point0 = math_utils::offset(math_utils::leftBottom(m_absoluteRect),
        {
            roundRadiusX, appearance().bottomLine.bottomOffset
        });
        auto point1 = math_utils::offset(math_utils::rightBottom(m_absoluteRect),
        {
            -roundRadiusX, appearance().bottomLine.bottomOffset
        });
        auto brush = resource_utils::solidColorBrush();
        float strokeWidth = appearance().bottomLine.strokeWidth;

        rndr->d2d1DeviceContext()->DrawLine(point0, point1, brush, strokeWidth);
    }

    void RawTextInput::onSizeHelper(SizeEvent& e)
    {
        // The text layout should adapt the visible text area.
        Panel::onSizeHelper(e); // LabelArea::onSizeHelper(e);
    }

    void RawTextInput::onChangeThemeStyleHelper(const ThemeStyle& style)
    {
        LabelArea::onChangeThemeStyleHelper(style);
        {
            auto& color = m_placeholder->appearance().foreground.color;
            if (style.name == L"Light")
            {
                color = D2D1::ColorF{ 0x8c8c8c };
            }
            else if (style.name == L"Dark")
            {
                color = D2D1::ColorF{ 0xa6a6a6 };
            }
        }
        appearance().changeTheme(Label::appearance(), style.name);
    }

    void RawTextInput::onKeyboardHelper(KeyboardEvent& e)
    {
        LabelArea::onKeyboardHelper(e);

        THROW_IF_NULL(Application::g_app);

        if (holdKeyboardFocus() && e.state.pressed())
        {
            switch (e.vkey)
            {
            case VK_BACK:
            {
                if (editable)
                {
                    if (m_selectedRange.count > 0)
                    {
                        if (eraseTextHelper(m_selectedRange))
                        {
                            setCaretPosition(m_selectedRange.offset);
                            setSelectedRange({ 0, 0 });

                            onTextChanged(m_text);
                            onTextEdited(m_text);
                        }
                    }
                    else if (m_caretPosition > 0)
                    {
                        auto caretPosition = m_caretPosition;

                        if (eraseTextHelper({ m_caretPosition - 1, 1 }))
                        {
                            // If the caret is at the end of the text,
                            // eraseTextHelper will update its position.
                            if (m_caretPosition == caretPosition)
                            {
                                setCaretPosition(m_caretPosition - 1);
                            }
                            onTextChanged(m_text);
                            onTextEdited(m_text);
                        }
                    }
                }
                break;
            }
            case VK_RETURN:
            {
                if (multiline)
                {
                    if (editable)
                    {
                        editSelectedText(L"\n");
                    }
                    break;
                }
                [[fallthrough]];
            }
            case VK_ESCAPE:
            {
                auto focus = Application::FocusType::Keyboard;
                Application::g_app->focusUIObject(focus, nullptr);
                return; // Return directly to avoid unexpected caret blinking.
            }
            case VK_END:
            {
                setCaretPosition(m_text.size());
                setSelectedRange({ 0, 0 });
                break;
            }
            case VK_HOME:
            {
                setCaretPosition(0);
                setSelectedRange({ 0, 0 });
                break;
            }
            case VK_LEFT:
            {
                if (m_selectedRange.count == 0)
                {
                    if (m_caretPosition > 0)
                    {
                        setCaretPosition(m_caretPosition - 1);
                    }
                }
                else // move to range's leftmost
                {
                    setCaretPosition(m_selectedRange.offset);
                    setSelectedRange({ 0, 0 });
                }
                break;
            }
            case VK_RIGHT:
            {
                if (m_selectedRange.count == 0)
                {
                    setCaretPosition(m_caretPosition + 1);
                }
                else // move to range's rightmost
                {
                    setCaretPosition(m_selectedRange.offset + m_selectedRange.count);
                    setSelectedRange({ 0, 0 });
                }
                break;
            }
            case VK_DELETE:
            {
                if (editable)
                {
                    if (m_selectedRange.count > 0)
                    {
                        if (eraseTextHelper(m_selectedRange))
                        {
                            setCaretPosition(m_selectedRange.offset);
                            setSelectedRange({ 0, 0 });

                            onTextChanged(m_text);
                            onTextEdited(m_text);
                        }
                    }
                    else // delete single character
                    {
                        if (eraseTextHelper({ m_caretPosition, 1 }))
                        {
                            onTextChanged(m_text);
                            onTextEdited(m_text);
                        }
                    }
                }
                break;
            }
            default:
            {
                if (e.CTRL())
                {
                    if (editable)
                    {
                        switch (e.vkey)
                        {
                        case 'X': performCommandCutSelection(); break;
                        case 'V': performCommandPasteSelection(); break;
                        default: break;
                        }
                    }
                }
                break;
            }}
            m_caretBlinkingFlag = true;
            m_caretBlinkingElapsedSecs = 0.0f;
        }
    }

    void RawTextInput::onTextInputHelper(WstrViewRefer text)
    {
        TextInputObject::onTextInputHelper(text);

        if (editable)
        {
            // 1. Allow empty input, which can be used to erase selected text.
            //
            // 2. A long input usually comes from the input method engine,
            //    and we assume the result returned by the IME is displayable.
            //
            // 3. For a single character, just exclude the non-displayable ones
            //    (ASCII codes 0 to 32, i.e. characters before space).
            //
            if (text.size() != 1 || text[0] >= L' ') editSelectedText((Wstring)text);
        }
    }

    Optional<Wstring> RawTextInput::normalizeRawText(WstrRefer in)
    {
        if (multiline) return std::nullopt;

        auto lineBreakPos = in.find_first_of(L'\n');
        if (lineBreakPos == Wstring::npos)
        {
            return std::nullopt;
        }
        // Cuts off at any line break to keep single line.
        return in.substr(0, lineBreakPos);
    }

    size_t RawTextInput::hitTestCaretPosition(const D2D1_POINT_2F& sfpt)
    {
        return LabelArea::hitTestCaretPosition
        ({
            sfpt.x + m_textContentOffset.x - m_visibleTextRect.left,
            sfpt.y + m_textContentOffset.y - m_visibleTextRect.top
        });
    }
}
