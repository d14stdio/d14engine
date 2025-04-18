﻿#pragma once

#include "Common/Precompile.h"

#include "Common/CppLangUtils/EnumMagic.h"

#include "UIKit/Appearances/Appearance.h"
#include "UIKit/SolidStyle.h"

namespace d14engine::uikit::appearance
{
    struct TabCaption
    {
        enum class ButtonState { Idle, Hover, Down };

        struct Appearance : appearance::Appearance
        {
            static void initialize();

            void changeTheme(WstrRefer themeName) override;

            constexpr static auto g_stateCount =
                cpp_lang_utils::enumCount<ButtonState>;

            struct Title
            {
                float leftPadding = 12.0f;
                float rightPadding = 32.0f;
            }
            title = {};

            struct CloseX
            {
                struct Icon
                {
                    struct Geometry
                    {
                        D2D1_SIZE_F size = { 8.0f, 8.0f };
                        D2D1_POINT_2F offset = { 4.0f, 4.0f };
                    }
                    geometry = {};

                    SolidStyle background[g_stateCount] = {};

                    float strokeWidth = 1.0f;
                }
                icon = {};

                struct Button
                {
                    struct Geometry
                    {
                        D2D1_SIZE_F size = { 16.0f, 16.0f };
                        float rightOffset = -24.0f; // vertical center

                        float roundRadius = 4.0f;
                    }
                    geometry = {};

                    SolidStyle background[g_stateCount] = {};
                }
                button = {};
            }
            closeX = {};

            struct ThemeData
            {
                struct CloseX
                {
                    struct Icon
                    {
                        SolidStyle background[g_stateCount] = {};
                    }
                    icon = {};

                    struct Button
                    {
                        SolidStyle background[g_stateCount] = {};
                    }
                    button = {};
                }
                closeX = {};
            };
            _D14_SET_THEME_DATA_MAP_DECL
        }
        appearanceData = {};
    };
}
