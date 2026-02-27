#include "Common/Precompile.h"

#include "UIKit/PlatformUtils.h"

#include "Common/DirectXError.h"
#include "Common/MathUtils/2D.h"

#include "UIKit/Application.h"

namespace d14engine::uikit::platform_utils
{
    WstringView exePath()
    {
        wchar_t* wpgmptr = {};
        THROW_IF_FAILED(_get_wpgmptr(&wpgmptr));
        return wpgmptr;
    }

    WstringView exeName()
    {
        auto path = exePath();
        auto offset = path.find_last_of(L'\\') + 1;
        return path.substr(offset);
    }

    WstringView exeBaseName()
    {
        auto name = exeName();
        auto count = name.find_last_of(L'.');
        return name.substr(0, count);
    }

    WstringView exeDirectory()
    {
        auto path = exePath();
        auto count = path.find_last_of(L'\\') + 1;
        return path.substr(0, count);
    }

    float dpi() // DPI: Dots Per Inch
    {
        auto& app = Application::g_app;

        if (app != nullptr && app->createInfo.dpi.has_value())
        {
            return app->createInfo.dpi.value();
        }
        return (float)GetSystemDpiForProcess(GetCurrentProcess());
    }

    D2D1_SIZE_F scaledByDpi(const D2D1_SIZE_F& sz)
    {
        auto factor = dpi() / 96.0f;
        return
        {
            sz.width * factor, sz.height * factor
        };
    }

    D2D1_POINT_2F scaledByDpi(const D2D1_POINT_2F& pt)
    {
        auto factor = dpi() / 96.0f;
        return
        {
            pt.x * factor, pt.y * factor
        };
    }

    D2D1_RECT_F scaledByDpi(const D2D1_RECT_F& rc)
    {
        return math_utils::rect
        (
            scaledByDpi(math_utils::leftTop(rc)),
            scaledByDpi(math_utils::size(rc))
        );
    }

    D2D1_SIZE_L scaledByDpi(const D2D1_SIZE_L& sz)
    {
        auto factor = dpi() / 96.0f;
        return
        {
            math_utils::round<long>((float)sz.cx * factor),
            math_utils::round<long>((float)sz.cy * factor)
        };
    }

    D2D1_POINT_2L scaledByDpi(const D2D1_POINT_2L& pt)
    {
        auto factor = dpi() / 96.0f;
        return
        {
            math_utils::round((float)pt.x * factor),
            math_utils::round((float)pt.y * factor)
        };
    }

    D2D1_RECT_L scaledByDpi(const D2D1_RECT_L& rc)
    {
        return math_utils::rect
        (
            scaledByDpi(math_utils::leftTop(rc)),
            scaledByDpi(math_utils::size(rc))
        );
    }

    D2D1_SIZE_F restoredByDpi(const D2D1_SIZE_F& sz)
    {
        auto factor = 96.0f / dpi();
        return
        {
            sz.width * factor, sz.height * factor
        };
    }

    D2D1_POINT_2F restoredByDpi(const D2D1_POINT_2F& pt)
    {
        auto factor = 96.0f / dpi();
        return
        {
            pt.x * factor, pt.y * factor
        };
    }

    D2D1_RECT_F restoredByDpi(const D2D1_RECT_F& rc)
    {
        return math_utils::rect
        (
            restoredByDpi(math_utils::leftTop(rc)),
            restoredByDpi(math_utils::size(rc))
        );
    }

    D2D1_SIZE_L restoredByDpi(const D2D1_SIZE_L& sz)
    {
        auto factor = 96.0f / dpi();
        return
        {
            math_utils::round<long>((float)sz.cx * factor),
            math_utils::round<long>((float)sz.cy * factor)
        };
    }

    D2D1_POINT_2L restoredByDpi(const D2D1_POINT_2L& pt)
    {
        auto factor = 96.0f / dpi();
        return
        {
            math_utils::round((float)pt.x * factor),
            math_utils::round((float)pt.y * factor)
        };
    }

    D2D1_RECT_L restoredByDpi(const D2D1_RECT_L& rc)
    {
        return math_utils::rect
        (
            restoredByDpi(math_utils::leftTop(rc)),
            restoredByDpi(math_utils::size(rc))
        );
    }
}
