#pragma once

#include "Common/Precompile.h"

#include "Common/CppLangUtils/EnumMagic.h"

#include "UIKit/Appearances/Appearance.h"
#include "UIKit/SolidStyle.h"
#include "UIKit/StrokeStyle.h"

namespace d14engine::uikit::appearance
{
    struct TabGroup
    {
        enum class TabState { Idle, Hovered, Selected };

        enum class ButtonState { Idle, Hovered, Pressed };

        struct Appearance : appearance::Appearance
        {
            Appearance();
            static void initialize();

            void changeTheme(WstrRefer themeName) override;

            constexpr static auto g_tabStateCount =
                cpp_lang_utils::enumCount<TabState>;

            constexpr static auto g_buttonStateCount =
                cpp_lang_utils::enumCount<ButtonState>;

            SolidStyle background = {};
            StrokeStyle stroke = {};

            struct TabBar
            {
                struct Geometry
                {
                    float height = 32.0f;
                    float rightPadding = 30.0f;
                }
                geometry = {};

                struct Tab
                {
                    struct Main
                    {
                        struct Geometry
                        {
                            D2D1_SIZE_F size = {};
                            float roundRadius = {};
                        }
                        geometry = {};

                        SolidStyle background = {};
                    }
                    main[g_tabStateCount] = {};

                    D2D1_COLOR_F activeShadowColor = {};

                    float draggingThreshold = 10.0f;
                }
                tab = {};

                struct Separator
                {
                    struct Geometry
                    {
                        D2D1_SIZE_F size = { 1.0f, 16.0f };
                        D2D1_POINT_2F offset = { 0.0f, 4.0f };
                    }
                    geometry = {};

                    SolidStyle background = { D2D1::ColorF{ 0x808080 }, 1.0f };
                }
                separator = {};

                struct Overflow
                {
                    struct Icon
                    {
                        struct Geometry
                        {
                            struct TopRect
                            {
                                D2D1_SIZE_F size = { 10.0f, 2.0f };
                                D2D1_POINT_2F offset = { 4.0f, 4.0f };
                            }
                            topRect = {};

                            struct BottomTriangle
                            {
                                D2D1_POINT_2F points[3] =
                                {
                                    { 4.0f, 8.0f },
                                    { 14.0f, 8.0f },
                                    { 9.0f, 14.0f }
                                };
                            }
                            bottomTriangle = {};
                        }
                        geometry = {};

                        SolidStyle background[g_buttonStateCount] = {};
                    }
                    icon = {};

                    struct Button
                    {
                        struct Geometry
                        {
                            D2D1_SIZE_F size = { 18.0f, 18.0f };
                            D2D1_POINT_2F offset = { -24.0f, 3.0f };

                            float roundRadius = 4.0f;
                        }
                        geometry = {};

                        SolidStyle background[g_buttonStateCount] = {};
                    }
                    button = {};
                }
                overflow = {};
            }
            tabBar = {};

            SolidStyle maskWhenBelowDemotingWindow = {};

            struct ThemeData
            {
                struct Background
                {
                    D2D1_COLOR_F color = {};
                }
                background = {};

                struct Stroke
                {
                    D2D1_COLOR_F color = {};
                }
                stroke = {};

                struct TabBar
                {
                    struct Tab
                    {
                        struct Main
                        {
                            SolidStyle background = {};
                        }
                        main[cpp_lang_utils::enumCount<TabState>] = {};

                        D2D1_COLOR_F activeShadowColor = {};
                    }
                    tab = {};

                    struct Overflow
                    {
                        struct Icon
                        {
                            SolidStyle background[g_buttonStateCount] = {};
                        }
                        icon = {};

                        struct Button
                        {
                            SolidStyle background[g_buttonStateCount] = {};
                        }
                        button = {};
                    }
                    overflow = {};
                }
                tabBar = {};
            };
            _D14_SET_THEME_DATA_MAP_DECL
        }
        appearanceData = {};
    };
}
