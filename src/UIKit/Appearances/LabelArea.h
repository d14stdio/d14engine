#pragma once

#include "Common/Precompile.h"

#include "UIKit/Appearances/Appearance.h"
#include "UIKit/SolidStyle.h"
#include "UIKit/StrokeStyle.h"

namespace d14engine::uikit::appearance
{
    struct LabelArea
    {
        struct Appearance : appearance::Appearance
        {
            Appearance();
            static void initialize();

            void changeTheme(WstrRefer themeName) override;

            struct Caret
            {
                bool visible = true;

                SolidStyle background = {};

                struct Animation
                {
                    float blinkingSecs = 0.5f;
                }
                animation = {};
            }
            caret = {};

            struct Selection
            {
                SolidStyle background = {};
                StrokeStyle stroke = {};
            }
            selection = {};

            struct ThemeData
            {
                struct Caret
                {
                    struct Background
                    {
                        D2D1_COLOR_F color = {};
                    }
                    background = {};
                }
                caret = {};

                struct Selection
                {
                    struct Background
                    {
                        D2D1_COLOR_F color = {};
                    }
                    background = {};
                }
                selection = {};
            };
            _D14_SET_THEME_DATA_MAP_DECL
        }
        appearanceData = {};
    };
}
