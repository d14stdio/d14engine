#include "Common/Precompile.h"

#include "UIKit/Appearances/LabelArea.h"

namespace d14engine::uikit::appearance
{
    LabelArea::Appearance::Appearance()
    {
        selection.stroke.opacity = 0.0f;
    }

    void LabelArea::Appearance::initialize()
    {
        auto& light = (g_themeData[L"Light"] = {});
        {
            light.caret.background.color = D2D1::ColorF{ 0x000000 };
            light.selection.background.color = D2D1::ColorF{ 0xadd6ff };
        }
        auto& dark = (g_themeData[L"Dark"] = {});
        {
            dark.caret.background.color = D2D1::ColorF{ 0xffffff };
            dark.selection.background.color = D2D1::ColorF{ 0x264f78 };
        }
    }

    void LabelArea::Appearance::changeTheme(WstrRefer themeName)
    {
        _D14_FIND_THEME_DATA(themeName);

        _D14_UPDATE_THEME_DATA_1(caret.background.color);
        _D14_UPDATE_THEME_DATA_1(selection.background.color);
    }
    _D14_SET_THEME_DATA_MAP_IMPL(LabelArea)
}
