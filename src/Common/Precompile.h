﻿#pragma once

//////////////////////
// Standard Library //
//////////////////////

#include <algorithm>
#include <array>
#include <exception>
#include <functional>
#include <iomanip>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <sstream>
#include <string_view>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace d14engine
{
    template<typename T>
    using Function = std::function<T>;
    template<typename T>
    using FuncRefer = const Function<T>&;

    template<typename T>
    using Optional = std::optional<T>;
    template<typename T>
    using OptRefer = const Optional<T>&;

    template<typename T>
    using SharedPtr = std::shared_ptr<T>;
    template<typename T>
    using ShrdPtrRefer = const SharedPtr<T>&;

    using String = std::string;
    using StrRefer = const String&;

    using StringView = std::string_view;
    using StrViewRefer = const StringView&;

    using Thread = std::thread;
    using ThreadRefer = const std::thread&;

    template<typename T>
    using UniquePtr = std::unique_ptr<T>;
    template<typename T>
    using UniqPtrRefer = const UniquePtr<T>&;

    template<typename... Types>
    using Variant = std::variant<Types...>;
    template<typename... Types>
    using VarRefer = const Variant<Types...>&;

    template<typename T>
    using WeakPtr = std::weak_ptr<T>;
    template<typename T>
    using WeakPtrRefer = const WeakPtr<T>&;

    using Wstring = std::wstring;
    using WstrRefer = const Wstring&;

    using WstringView = std::wstring_view;
    using WstrViewRefer = const WstringView&;
}

///////////////////////////
// Windows & DirectX SDK //
///////////////////////////

#ifndef _D14_AGILITY_SDK
#define _D14_AGILITY_SDK true
#endif

#if _D14_AGILITY_SDK
#ifndef _D14_AGILITY_VER
#define _D14_AGILITY_VER 615
#endif
#ifndef _D14_AGILITY_PATH
#ifdef _WIN64
#define _D14_AGILITY_PATH "lib/DirectX/x64/"
#else
#define _D14_AGILITY_PATH "lib/DirectX/x86/"
#endif
#endif
#endif
// Refer to https://devblogs.microsoft.com/directx/gettingstarted-dx12agility/#header-include-order
// It is recommended to include Agility SDK headers before the Windows 10 SDK to avoid conflicts.
#include <d3d12.h>

#if _D14_AGILITY_SDK
#include "d3dx12/d3dx12.h"
#else
#include "d3dx12.h"
#endif

// Refer to https://devblogs.microsoft.com/directx/gettingstarted-dx12agility/#2.-set-agility-sdk-parameters
// We need to set Agility SDK parameters to help locate the custom D3D12Core.dll during building.
#if _D14_AGILITY_SDK
extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion; }
extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath; }
#endif

#include <comdef.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1effects_2.h>
#include <d3d11_1.h>
#include <d3d11on12.h>
#include <dcomp.h>
#include <dwmapi.h>
#include <dwrite_3.h>
#include <dxgi1_6.h>
#include <imm.h>
#include <shlwapi.h>
#include <wincodec.h>
#include <windows.h>
#include <windowsx.h>
#include <wrl.h>

#include <DirectXColors.h>
#include <DirectXMath.h>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#include "DirectXShaderCompiler/dxcapi.h"

namespace d14engine
{
    using namespace DirectX;

    using Microsoft::WRL::ComPtr;
    template<typename T>
    using ComPtrParam = const ComPtr<T>&;
}

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dcomp.lib")
#pragma comment(lib, "delayimp.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "WindowsCodecs.lib")

#pragma comment(lib, "dxcompiler.lib")

///////////////////
// Miscellaneous //
///////////////////

constexpr size_t operator""_uz(unsigned long long num)
{
    return (size_t)num;
}
// Allows virtual inheritance to solve diamond problem.
#pragma warning(disable : 4250)
