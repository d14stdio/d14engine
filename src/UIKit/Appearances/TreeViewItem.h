﻿#pragma once

#include "Common/Precompile.h"

#include "Common/CppLangUtils/EnumMagic.h"

#include "UIKit/Appearances/Appearance.h"
#include "UIKit/Event.h"
#include "UIKit/SolidStyle.h"

namespace d14engine::uikit
{
    struct TreeViewItemState
    {
        enum class Flag { Folded, Unfolded } flag = {};

        size_t index() const { return (size_t)flag; }
    };

    struct TreeViewItemStateChangeEvent : uikit::Event
    {
        TreeViewItemState::Flag flag = {};

        bool folded() const { return flag == TreeViewItemState::Flag::Folded; }
        bool unfolded() const { return flag == TreeViewItemState::Flag::Unfolded; }

        // The ancestor-flag is UNFOLDED if and only if all ancestors of the
        // item are UNFOLDED (not only the immediate parent), and one can be
        // activated only when its ancestor-flag is UNFOLDED.
        TreeViewItemState::Flag ancestorFlag = {};

        bool ancestorFolded() const { return ancestorFlag == TreeViewItemState::Flag::Folded; }
        bool ancestorUnfolded() const { return ancestorFlag == TreeViewItemState::Flag::Unfolded; }

        bool operator==(const TreeViewItemStateChangeEvent& other) const
        { return flag == other.flag && ancestorFlag == other.ancestorFlag; }
    };
}

namespace d14engine::uikit::appearance
{
    struct TreeViewItem
    {
        struct Appearance : appearance::Appearance
        {
            static void initialize();

            void changeTheme(WstrRefer themeName) override;

            constexpr static auto g_stateCount =
                cpp_lang_utils::enumCount<TreeViewItemState::Flag>;

            struct Arrow
            {
                struct Geometry
                {
                    struct Line
                    {
                        D2D1_POINT_2F point0 = {}, point1 = {};
                    }
                    line0 = {}, line1 = {};
                }
                geometry[g_stateCount] =
                {
                    // Foled
                    {
                        { { 8.0f, 7.0f }, { 16.0f, 15.0f } }, // line 0
                        { { 16.0f, 15.0f }, { 8.0f, 23.0f } } // line 1
                    },
                    // Unfolded
                    {
                        { { 5.0f, 11.0f }, { 12.0f, 19.0f } }, // line 0
                        { { 12.0f, 19.0f }, { 19.0f, 11.0f } } // line 1
                    }
                };

                SolidStyle background = {};
                // i.e. background when disabled
                SolidStyle secondaryBackground = {};

                float strokeWidth = 2.0f;
            }
            arrow = {};

            struct ThemeData
            {
                struct Arrow
                {
                    struct Background
                    {
                        D2D1_COLOR_F color = {};
                    }
                    background = {},
                    secondaryBackground = {};
                }
                arrow = {};
            };
            _D14_SET_THEME_DATA_MAP_DECL
        }
        appearanceData = {};
    };
}
