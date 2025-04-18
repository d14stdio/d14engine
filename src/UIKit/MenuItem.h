﻿#pragma once

#include "Common/Precompile.h"

#include "UIKit/Appearances/MenuItem.h"
#include "UIKit/ViewItem.h"

namespace d14engine::uikit
{
    struct PopupMenu;

    struct MenuItem : appearance::MenuItem, ViewItem
    {
        friend PopupMenu;

        using ViewItem::ViewItem;

        _D14_SET_APPEARANCE_PROPERTY(MenuItem)

    protected:
        WeakPtr<PopupMenu> m_parentMenu = {};

        // Since the associated menu is created as a root UI object, it is
        // not destroyed after being disassociated with the menu-item, which
        // makes it convenient to transfer the menu between different items.
        // If you do not need the menu anymore, please call Panel::release(),
        // otherwise the memory leaks.
        SharedPtr<PopupMenu> m_associatedMenu = {};

    public:
        const WeakPtr<PopupMenu>& parentMenu() const;

        WeakPtr<PopupMenu> associatedMenu() const;
        void setAssociatedMenu(ShrdPtrRefer<PopupMenu> menu);

    public:
        // Controls whether to close parent-menus immediately after clicking.
        // It is useful when implementing something like checkable-menu-item.
        bool isInstant = true;

        void popupAssociatedMenu();

        // Controls whether to send the related trigger event after clicking.
        // This is often set as false for the static item like MenuSeparator.
        bool isTriggerItem = true;

    protected:
        // IDrawObject2D
        void onRendererDrawD2d1ObjectHelper(renderer::Renderer* rndr) override;

        // Panel
        void onChangeThemeStyleHelper(const ThemeStyle& style) override;

        void onMouseButtonHelper(MouseButtonEvent& e) override;
    };
}
