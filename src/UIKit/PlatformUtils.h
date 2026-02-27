#pragma once

#include "Common/Precompile.h"

#include "Common/MathUtils/2D.h"
#include "Common/MathUtils/Basic.h"

namespace d14engine::uikit::platform_utils
{
#pragma region EXE Properties

    WstringView exePath();
    WstringView exeName();
    WstringView exeBaseName();
    WstringView exeDirectory();

#pragma endregion

#pragma region DPI Scaling

    float dpi(); // DPI: Dots Per Inch

    template<typename RT = float, typename T>
    RT scaledByDpi(T value)
    {
        auto factor = dpi() / 96.0f;
        return math_utils::round<RT>((float)value * factor);
    }
    D2D1_SIZE_F scaledByDpi(const D2D1_SIZE_F& size);
    D2D1_POINT_2F scaledByDpi(const D2D1_POINT_2F& point);
    D2D1_RECT_F scaledByDpi(const D2D1_RECT_F& rect);

    D2D1_SIZE_L scaledByDpi(const D2D1_SIZE_L& size);
    D2D1_POINT_2L scaledByDpi(const D2D1_POINT_2L& point);
    D2D1_RECT_L scaledByDpi(const D2D1_RECT_L& rect);

    template<typename RT = float, typename T>
    RT restoredByDpi(T value)
    {
        auto factor = 96.0f / dpi();
        return math_utils::round<RT>((float)value * factor);
    }
    D2D1_SIZE_F restoredByDpi(const D2D1_SIZE_F& size);
    D2D1_POINT_2F restoredByDpi(const D2D1_POINT_2F& point);
    D2D1_RECT_F restoredByDpi(const D2D1_RECT_F& rect);

    D2D1_SIZE_L restoredByDpi(const D2D1_SIZE_L& size);
    D2D1_POINT_2L restoredByDpi(const D2D1_POINT_2L& point);
    D2D1_RECT_L restoredByDpi(const D2D1_RECT_L& rect);

#pragma endregion

}
