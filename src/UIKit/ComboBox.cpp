#include "Common/Precompile.h"

#include "UIKit/ComboBox.h"

#include "Common/DirectXError.h"

#include "UIKit/IconLabel.h"
#include "UIKit/Label.h"
#include "UIKit/PopupMenu.h"

using namespace d14engine::renderer;

namespace d14engine::uikit
{
    ComboBox::ComboBox(float roundRadius, const D2D1_RECT_F& rect)
        :
        Panel(rect, resource_utils::solidColorBrush()),
        FlatButton(IconLabel::comboBoxLayout(), roundRadius, rect)
    {
        // Here left blank intentionally.
    }

    void ComboBox::onInitializeFinish()
    {
        FlatButton::onInitializeFinish();

        arrowIcon.loadStrokeStyle();

        m_dropDownMenu = makeRootUIObject<PopupMenu>();

        m_dropDownMenu->f_onTriggerMenuItem = [this](PopupMenu* menu, size_t index)
        {
            setSelected(index);
        };
        m_dropDownMenu->setBackgroundTriggerPanel(true);

        auto& geometry = m_dropDownMenu->appearance().geometry;
        geometry.extension = geometry.roundRadius = roundRadiusX;

        m_dropDownMenu->setSize(width(), m_dropDownMenu->height());
        m_dropDownMenu->setPosition(math_utils::leftBottom(m_absoluteRect));
    }

    void ComboBox::ArrowIcon::loadStrokeStyle()
    {
        THROW_IF_NULL(Application::g_app);

        auto& app = Application::g_app;
        auto factory = app->renderer()->d2d1Factory();

        auto prop = D2D1::StrokeStyleProperties
        (
        /* startCap */ D2D1_CAP_STYLE_ROUND,
        /* endCap   */ D2D1_CAP_STYLE_ROUND,
        /* dashCap  */ D2D1_CAP_STYLE_ROUND
        );
        auto& style = strokeStyle;

        THROW_IF_FAILED(factory->CreateStrokeStyle
        (
        /* strokeStyleProperties */ prop,
        /* dashes                */ nullptr,
        /* dashesCount           */ 0,
        /* strokeStyle           */ &style
        ));
    }

    void ComboBox::onSelectedChange(OptRefer<size_t> index)
    {
        onSelectedChangeHelper(index);

        if (f_onSelectedChange) f_onSelectedChange(this, index);
    }

    void ComboBox::onSelectedChangeHelper(OptRefer<size_t> index)
    {
        // This method intentionally left blank.
    }

    ShrdPtrRefer<PopupMenu> ComboBox::dropDownMenu() const
    {
        return m_dropDownMenu;
    }

    void ComboBox::setDropDownMenu(ShrdPtrRefer<PopupMenu> menu)
    {
        if (menu && !cpp_lang_utils::isMostDerivedEqual(menu, m_dropDownMenu))
        {
            setSelected(std::nullopt);

            m_dropDownMenu->release();
            m_dropDownMenu = menu;
        }
    }

    OptRefer<size_t> ComboBox::selectedIndex() const
    {
        return m_selectedIndex;
    }

    void ComboBox::setSelected(OptRefer<size_t> index)
    {
        auto& items = m_dropDownMenu->items();
        if (index.has_value() && index.value() < items.size())
        {
            auto& item = items[index.value()];
            auto content = item->getContent<IconLabel>().lock();

            if (content != nullptr)
            {
                m_content->icon = content->icon;

                auto label = content->label().get();
                m_content->label()->copyTextStyle(label, label->text());
            }
        }
        else // Typically, pass nullopt to clear current selected item.
        {
            m_content->icon = {};
            m_content->label()->setText({});
        }
        m_content->updateLayout();

        if (index != m_selectedIndex)
        {
            onSelectedChange(index);
            m_selectedIndex = index;
        }
    }

    void ComboBox::onRendererDrawD2d1ObjectHelper(Renderer* rndr)
    {
        FlatButton::onRendererDrawD2d1ObjectHelper(rndr);

        /////////////////////
        // Drop-down Arrow //
        /////////////////////

        auto& setting = appearance().arrow;
        auto& geometry = setting.geometry;

        auto& background = m_enabled ? setting.background : setting.secondaryBackground;

        resource_utils::solidColorBrush()->SetColor(background.color);
        resource_utils::solidColorBrush()->SetOpacity(background.opacity);

        auto arrowOrigin = math_utils::rightTop(m_absoluteRect);

        rndr->d2d1DeviceContext()->DrawLine
        (
        /* point0      */ math_utils::offset(arrowOrigin, geometry.line0.point0),
        /* point1      */ math_utils::offset(arrowOrigin, geometry.line0.point1),
        /* brush       */ resource_utils::solidColorBrush(),
        /* strokeWidth */ setting.strokeWidth,
        /* strokeStyle */ arrowIcon.strokeStyle.Get()
        );
        rndr->d2d1DeviceContext()->DrawLine
        (
        /* point0      */ math_utils::offset(arrowOrigin, geometry.line1.point0),
        /* point1      */ math_utils::offset(arrowOrigin, geometry.line1.point1),
        /* brush       */ resource_utils::solidColorBrush(),
        /* strokeWidth */ setting.strokeWidth,
        /* strokeStyle */ arrowIcon.strokeStyle.Get()
        );
    }

    void ComboBox::onSizeHelper(SizeEvent& e)
    {
        FlatButton::onSizeHelper(e);

        m_dropDownMenu->setSize(e.size.width, m_dropDownMenu->height());
    }

    void ComboBox::onChangeThemeStyleHelper(const ThemeStyle& style)
    {
        Button::onChangeThemeStyleHelper(style);

        appearance().changeTheme(FlatButton::appearance(), style.name);
    }

    void ComboBox::onMouseButtonReleaseHelper(ClickablePanel::Event& e)
    {
        FlatButton::onMouseButtonReleaseHelper(e);

        if (e.left())
        {
            if (!menuOffset.has_value())
            {
                float offsetY = 0.0f;
                if (m_selectedIndex.has_value())
                {
                    auto& item = m_dropDownMenu->items()[m_selectedIndex.value()];
                    offsetY = -item->position().y;
                }
                m_dropDownMenu->setPosition(math_utils::offset(absolutePosition(), { 0.0f, offsetY }));
            }
            else m_dropDownMenu->setPosition(selfCoordToAbsolute(menuOffset.value()));

            m_dropDownMenu->setActivated(true);
        }
    }
}
