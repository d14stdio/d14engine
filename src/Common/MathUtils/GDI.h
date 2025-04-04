﻿#pragma once

#include "Common/Precompile.h"

namespace d14engine::math_utils
{
    RECT rect(LONG x, LONG y, LONG width, LONG height);
    RECT rect(const POINT& point, const SIZE& size);

    LONG width(const RECT& rect);
    LONG height(const RECT& rect);
    SIZE size(const RECT& rect);

    POINT center(const RECT& rect);
    POINT leftTop(const RECT& rect);
    POINT leftBottom(const RECT& rect);
    POINT rightTop(const RECT& rect);
    POINT rightBottom(const RECT& rect);

    // Example 1: dst = { 0, 0, 10, 10 }, src = { 8, 8 }, return = { 1, 1, 9, 9 }.
    // Example 2: dst = { 2, 3, 18, 15 }, src = { 4, 4 }, return = { 8, 7, 12, 11 }.
    RECT centered(const RECT& dst, const SIZE& src);
}
