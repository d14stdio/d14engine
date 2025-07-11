﻿#include "Common/Precompile.h"

#include "UIKit/MenuItem.h"

#include "Common/CppLangUtils/PointerCompare.h"

#include "UIKit/PopupMenu.h"

using namespace d14engine::renderer;

namespace d14engine::uikit
{
    const WeakPtr<PopupMenu>& MenuItem::parentMenu() const
    {
        return m_parentMenu;
    }

    WeakPtr<PopupMenu> MenuItem::associatedMenu() const
    {
        return m_associatedMenu;
    }

    void MenuItem::setAssociatedMenu(ShrdPtrRefer<PopupMenu> menu)
    {
        // cpp_lang_utils::isMostDerivedEqual treats 2 empty objects as equal,
        // so check "menu" here to make sure the associated menu can be reset.
        if (menu && cpp_lang_utils::isMostDerivedEqual(menu, m_parentMenu.lock())) return;

        if (m_associatedMenu)
        {
            m_associatedMenu->m_associatedItem.reset();
        }
        m_associatedMenu = menu;

        if (m_associatedMenu)
        {
            m_associatedMenu->m_associatedItem = std::static_pointer_cast<MenuItem>(shared_from_this());
        }
    }

    void MenuItem::popupAssociatedMenu()
    {
        if (m_associatedMenu)
        {
            if (m_associatedMenu->isHorzConstraintMeet(m_absoluteRect.right))
            {
                m_associatedMenu->setPosition(m_absoluteRect.right, m_absoluteRect.top);
            }
            else m_associatedMenu->setPosition(m_absoluteRect.left - m_associatedMenu->width(), m_absoluteRect.top);

            m_associatedMenu->setActivated(true);
        }
    }

    void MenuItem::onRendererDrawD2d1ObjectHelper(Renderer* rndr)
    {
        ViewItem::onRendererDrawD2d1ObjectHelper(rndr);

        //////////////////
        // Expand Arrow //
        //////////////////

        if (m_associatedMenu)
        {
            auto& setting = appearance().arrow;

            resource_utils::solidColorBrush()->SetColor(setting.background.color);
            resource_utils::solidColorBrush()->SetOpacity(setting.background.opacity);

            auto rigthTop = math_utils::rightTop(m_absoluteRect);
            auto arrowOrg = math_utils::offset(rigthTop, { setting.geometry.rightOffset, 0.0f });

            rndr->d2d1DeviceContext()->DrawLine
            (
            /* point0      */ math_utils::offset(arrowOrg, setting.geometry.line0.point0),
            /* point1      */ math_utils::offset(arrowOrg, setting.geometry.line0.point1),
            /* brush       */ resource_utils::solidColorBrush(),
            /* strokeWidth */ setting.strokeWidth
            );
            rndr->d2d1DeviceContext()->DrawLine
            (
            /* point0      */ math_utils::offset(arrowOrg, setting.geometry.line1.point0),
            /* point1      */ math_utils::offset(arrowOrg, setting.geometry.line1.point1),
            /* brush       */ resource_utils::solidColorBrush(),
            /* strokeWidth */ setting.strokeWidth
            );
        }
    }

    void MenuItem::onChangeThemeStyleHelper(const ThemeStyle& style)
    {
        ViewItem::onChangeThemeStyleHelper(style);

        appearance().changeTheme(ViewItem::appearance(), style.name);
    }

    void MenuItem::onMouseButtonHelper(MouseButtonEvent& e)
    {
        ViewItem::onMouseButtonHelper(e);

        if (e.state.leftUp())
        {
            // Note an expandable menu-item should not trigger any command.
            if (!m_associatedMenu && isInstant && isTriggerItem && !m_parentMenu.expired())
            {
                m_parentMenu.lock()->setActivatedWithAncestors(false);
            }
        }
    }
}
