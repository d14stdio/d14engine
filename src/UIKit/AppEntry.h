﻿#pragma once

#include "Common/Precompile.h"

// Do NOT remove this header for code tidy
// as D14_SET_APP_ENTRY uses RuntimeError.
#include "Common/RuntimeError.h"

#pragma comment(linker, "/subsystem:windows")
#pragma comment(linker, "/entry:wmainCRTStartup")

// The entry function must be of the specific signature:
// ----- int Func_Name(int argc, wchar_t* argv[]) -----
#define D14_SET_APP_ENTRY(Func_Name) \
int Func_Name(int _, wchar_t* __[]); \
int wmain(int argc, wchar_t* argv[]) \
{ \
    try \
    { \
        return Func_Name(argc, argv); \
    } \
    catch (std::exception& e) \
    { \
        MessageBoxA(nullptr, e.what(), "std::exception", MB_OK | MB_ICONERROR); \
    } \
    catch (d14engine::RuntimeError& e) \
    { \
        MessageBoxW(nullptr, e.message().c_str(), L"d14engine::RuntimeError", MB_OK | MB_ICONERROR); \
    } \
    catch (...) \
    { \
        MessageBoxW(nullptr, L"Unknown exception encountered.", L"Unknown Error", MB_OK | MB_ICONERROR); \
    } \
    exit(EXIT_FAILURE); \
} \
int Func_Name(int argc, wchar_t* argv[])
