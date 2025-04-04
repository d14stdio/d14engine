﻿#pragma once

#include "Common/Precompile.h"

#include "Common/CppLangUtils/EnumMagic.h"

#include "UIKit/Appearances/Appearance.h"
#include "UIKit/Event.h"
#include "UIKit/SolidStyle.h"
#include "UIKit/StrokeStyle.h"

namespace d14engine::uikit
{
    struct CheckBoxState
    {
        enum class ActiveFlag { Unchecked, Intermediate, Checked } activeFlag = ActiveFlag::Unchecked;
        enum class ButtonFlag { Idle, Hover, Down, Disabled } buttonFlag = ButtonFlag::Idle;

        size_t index() const
        {
            return (size_t)buttonFlag + (size_t)activeFlag * cpp_lang_utils::enumCount<ButtonFlag>;
        }
        enum class Flag
        {
            UncheckedIdle, UncheckedHover, UncheckedDown, UncheckedDisabled,
            IntermediateIdle, IntermediateHover, IntermediateDown, IntermediateDisabled,
            CheckedIdle, CheckedHover, CheckedDown, CheckedDisabled
        };
    };

    struct CheckBoxStateChangeEvent : Event
    {
        CheckBoxState::ActiveFlag flag = CheckBoxState::ActiveFlag::Unchecked;

        bool operator==(const CheckBoxStateChangeEvent& other) const { return flag == other.flag; }

        bool unchecked() const { return flag == CheckBoxState::ActiveFlag::Unchecked; }
        bool intermediate() const { return flag == CheckBoxState::ActiveFlag::Intermediate; }
        bool checked() const { return flag == CheckBoxState::ActiveFlag::Checked; }
    };
}

namespace d14engine::uikit::appearance
{
    struct CheckBox
    {
        struct Appearance : appearance::Appearance
        {
            static void initialize();

            void changeTheme(WstrRefer themeName) override;

            constexpr static auto g_stateCount =
                cpp_lang_utils::enumCount<CheckBoxState::Flag>;

            struct Icon
            {
                struct Geometry
                {
                    struct Intermediate
                    {
                        D2D1_SIZE_F size = { 12.0f, 2.0f };
                    }
                    intermediate = {};

                    struct Checked
                    {
                        struct Line
                        {
                            D2D1_POINT_2F point0 = {}, point1 = {};
                        }
                        tickLine0 = { { 5.0f, 12.0f }, { 10.0f, 17.0f } },
                        tickLine1 = { { 10.0f, 17.0f }, { 19.0f, 7.0f } };

                        float strokeWidth = 2.0f;
                    }
                    checked = {};
                }
                geometry = {};

                SolidStyle background[g_stateCount] = {};
            }
            icon = {};

            struct Button
            {
                SolidStyle background = {};
                StrokeStyle stroke = {};
            }
            button[g_stateCount] = {};

            struct ThemeData
            {
                struct Icon
                {
                    SolidStyle background[g_stateCount] = {};
                }
                icon = {};

                struct Button
                {
                    SolidStyle background = {};
                    StrokeStyle stroke = {};
                }
                button[g_stateCount] = {};
            };
            _D14_SET_THEME_DATA_MAP_DECL
        }
        appearanceData = {};
    };
}
