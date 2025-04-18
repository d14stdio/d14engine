﻿#include "Common/Precompile.h"

#include "UIKit/FileSystemUtils.h"

namespace d14engine::file_system_utils
{
    Wstring extractFileName(WstrRefer path)
    {
        auto slashPos = path.find_last_of(L'/');
        if (slashPos != Wstring::npos)
        {
            return path.substr(slashPos + 1);
        }
        else // Try search back-slash separator.
        {
            slashPos = path.find_last_of(L'\\');
            if (slashPos != Wstring::npos)
            {
                return path.substr(slashPos + 1);
            }
        }
        return path;
    }

    Wstring extractFilePrefix(WstrRefer fileName, WCHAR token)
    {
        auto tokenPos = fileName.find_last_of(token);
        if (tokenPos != Wstring::npos)
        {
            return fileName.substr(0, tokenPos);
        }
        return fileName;
    }

    Wstring extractFileSuffix(WstrRefer fileName, WCHAR token)
    {
        auto tokenPos = fileName.find_last_of(token);
        if (tokenPos != Wstring::npos)
        {
            return fileName.substr(tokenPos + 1);
        }
        return fileName;
    }

    bool foreachFileInDir(WstrRefer dir, WstrRefer wildcard, const FileOperationFunc& func)
    {
        WIN32_FIND_DATA findData = {};
        auto handle = FindFirstFileW((dir + wildcard).c_str(), &findData);
        if (handle == INVALID_HANDLE_VALUE)
        {
            return false;
        }
        bool nextFileFound = true;
        while (nextFileFound)
        {
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                if (func(dir + findData.cFileName)) break;
            }
            nextFileFound = FindNextFileW(handle, &findData);
        }
        return FindClose(handle);
    }
}
