﻿#pragma once

#include "Common/Precompile.h"

#include "UIKit/Appearances/PopupMenu.h"
#include "UIKit/MenuItem.h"
#include "UIKit/ShadowMask.h"
#include "UIKit/WaterfallView.h"

namespace d14engine::uikit
{
    // A popup menu should be created as a root UI object, thus it can
    // be activated at any position. The design idea is that the menu is
    // used to perform some shortcuts and can be deactivated immediately
    // after use, so it is a bad idea to make the menu managed by others.

    struct PopupMenu : appearance::PopupMenu, WaterfallView<MenuItem>
    {
        explicit PopupMenu(const D2D1_RECT_F& rect = {});

        ShadowMask shadow = {};

        D2D1_SIZE_F extendedSize(const D2D1_SIZE_F& rawSize) const;
        D2D1_SIZE_F narrowedSize(const D2D1_SIZE_F& rawSize) const;

        _D14_SET_APPEARANCE_PROPERTY(PopupMenu)

    public:
        void onChangeActivity(bool value);

        Function<void(PopupMenu*, bool)> f_onChangeActivity = {};

        void onTriggerMenuItem(size_t index);

        Function<void(PopupMenu*, size_t)> f_onTriggerMenuItem = {};

    protected:
        virtual void onChangeActivityHelper(bool value);

        virtual void onTriggerMenuItemHelper(size_t index);

    public:
        void insertItem(const ItemArray& items, size_t index = 0) override;
        void appendItem(const ItemArray& items);

        void removeItem(size_t index, size_t count = 1) override;
        void clearAllItems() override;

    protected:
        WeakPtr<MenuItem> m_associatedItem = {};

        friend void MenuItem::setAssociatedMenu(ShrdPtrRefer<PopupMenu> menu);

    public:
        const WeakPtr<MenuItem>& associatedItem() const;

        void setActivated(bool value);

        void setActivatedWithAncestors(bool value);
        void setActivatedWithDescendants(bool value);

    public:
        D2D1_RECT_F constrainedRectangle = math_utils::infiniteRectF();

        bool isHorzConstraintMeet(float expectedHorzOffset);
        bool isVertConstraintMeet(float expectedVertOffset);

        void showInConstrainedRect(D2D1_POINT_2F expectedOffset);

    protected:
        // Used by the top menu to close itself when clicking out of the area.
        SharedPtr<Panel> m_backgroundTriggerPanel = {};

    public:
        WeakPtr<Panel> backgroundTriggerPanel() const;
        void setBackgroundTriggerPanel(bool value);

    protected:
        // IDrawObject2D
        void onRendererDrawD2d1LayerHelper(renderer::Renderer* rndr) override;

        void onRendererDrawD2d1ObjectHelper(renderer::Renderer* rndr) override;

        // Panel
        void onSizeHelper(SizeEvent& e) override;

        void onChangeThemeStyleHelper(const ThemeStyle& style) override;

        void onMouseMoveHelper(MouseMoveEvent& e) override;

        void onMouseLeaveHelper(MouseMoveEvent& e) override;

        void onMouseButtonHelper(MouseButtonEvent& e) override;
    };
}
