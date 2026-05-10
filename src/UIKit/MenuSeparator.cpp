#include "Common/Precompile.h"

#include "UIKit/MenuSeparator.h"

#include "Common/MathUtils/2D.h"

#include "UIKit/ResourceUtils.h"

using namespace d14engine::renderer;

namespace d14engine::uikit
{
    MenuSeparator::MenuSeparator(const D2D1_RECT_F& rect)
        :
        MenuItem((ShrdPtrRefer<Panel>)nullptr, rect) { isTriggerItem = false; }


    void MenuSeparator::onRendererDrawD2d1ObjectHelper(Renderer* rndr)
    {
        auto& background = appearance().background;

        resource_utils::solidColorBrush()->SetColor(background.color);
        resource_utils::solidColorBrush()->SetOpacity(background.opacity);

        rndr->d2d1DeviceContext()->DrawLine
        (
        /* point0      */ math_utils::leftCenter(m_absoluteRect),
        /* point1      */ math_utils::rightCenter(m_absoluteRect),
        /* brush       */ resource_utils::solidColorBrush(),
        /* strokeWidth */ appearance().strokeWidth
        );
    }

    void MenuSeparator::onChangeThemeStyleHelper(const ThemeStyle& style)
    {
        MenuItem::onChangeThemeStyleHelper(style);

        appearance().changeTheme(ViewItem::appearance(), style.name);
    }
}
