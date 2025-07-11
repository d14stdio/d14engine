﻿#pragma once

#include "Common/Precompile.h"

#include "UIKit/Appearances/Appearance.h"
#include "UIKit/SolidStyle.h"

namespace d14engine::uikit::appearance
{
    struct PopupMenu
    {
        struct Appearance : appearance::Appearance
        {
            static void initialize();

            void changeTheme(WstrRefer themeName) override;

            struct Geometry
            {
                // Call loadShadowBitmap after changing this field.
                float extension = 5.0f;

                float roundRadius = 5.0f;
            }
            geometry = {};

            SolidStyle background = {};

            struct Shadow
            {
                D2D1_RECT_F offset = { 2.0f, 4.0f, -2.0f, 0.0f };

                D2D1_COLOR_F color = {};

                float standardDeviation = 5.0f;
            }
            shadow = {};

            struct ThemeData
            {
                SolidStyle background = {};

                struct Shadow
                {
                    D2D1_COLOR_F color = {};
                }
                shadow = {};
            };
            _D14_SET_THEME_DATA_MAP_DECL
        }
        appearanceData = {};
    };
}
