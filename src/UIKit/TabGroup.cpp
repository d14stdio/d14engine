#include "Common/Precompile.h"

#include "UIKit/TabGroup.h"

#include "Common/CppLangUtils/PointerCompare.h"
#include "Common/DirectXError.h"

#include "UIKit/PopupMenu.h"
#include "UIKit/TabCaption.h"
#include "UIKit/Window.h"

using namespace d14engine::renderer;

namespace d14engine::uikit
{
    TabGroup::TabGroup(const D2D1_RECT_F& rect)
        :
        Panel(rect, resource_utils::solidColorBrush()),
        ResizablePanel(rect, resource_utils::solidColorBrush())
    {
        // Here left blank intentionally.
    }

    TabGroup::~TabGroup()
    {
        // No need to do the clearing if the application already destroyed.
        if (Application::g_app != nullptr) m_overflowMenu->release();
    }

    void TabGroup::onInitializeFinish()
    {
        ResizablePanel::onInitializeFinish();

        ////////////////////
        // Initialization //
        ////////////////////

        transform(math_utils::adaptMaxSize(m_rect, minimalSize()));
        setResizable(false);

        //////////////////////
        // Cached Resources //
        //////////////////////

        selectedTabRes.loadMask();
        selectedTabRes.loadPathGeo();

        overflowButtonRes.loadMask();
        overflowButtonRes.loadPathGeo();

        ///////////////////
        // Overflow Menu //
        ///////////////////

        m_overflowMenu = makeRootUIObject<PopupMenu>();
        m_overflowMenu->setBackgroundTriggerPanel(true);
    }

    void TabGroup::SelectedTabRes::loadMask()
    {
        TabGroup* tg = m_master;
        THROW_IF_NULL(tg);

        auto& setting = tg->appearance().tabBar.tab.main[(size_t)TabState::Selected];

        mask.loadBitmap(setting.geometry.size);
    }

    void TabGroup::SelectedTabRes::loadPathGeo()
    {
        TabGroup* tg = m_master;
        THROW_IF_NULL(tg);

        THROW_IF_NULL(Application::g_app);

        auto factory = Application::g_app->renderer()->d2d1Factory();
        THROW_IF_FAILED(factory->CreatePathGeometry(&pathGeo));

        ComPtr<ID2D1GeometrySink> geoSink = {};
        THROW_IF_FAILED(pathGeo->Open(&geoSink));
        {
            auto& setting = tg->appearance().tabBar.tab.main[(size_t)TabState::Selected];

            auto& tabWidth = setting.geometry.size.width;
            auto& tabHeight = setting.geometry.size.height;
            auto& tabRoundRadius = setting.geometry.roundRadius;

            geoSink->BeginFigure({ 0.0f, tabHeight }, D2D1_FIGURE_BEGIN_FILLED);

            //------------------------------------------------------------------
            // Left Bottom Corner
            //------------------------------------------------------------------

            geoSink->AddArc(
            {
                .point          = { tabRoundRadius, tabHeight - tabRoundRadius },
                .size           = { tabRoundRadius, tabRoundRadius },
                .rotationAngle  = 90.0f,
                .sweepDirection = D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
                .arcSize        = D2D1_ARC_SIZE_SMALL
            });
            geoSink->AddLine({ tabRoundRadius, tabRoundRadius });

            //------------------------------------------------------------------
            // Left Top Corner
            //------------------------------------------------------------------

            geoSink->AddArc(
            {
                .point          = { tabRoundRadius * 2.0f, 0.0f },
                .size           = { tabRoundRadius, tabRoundRadius },
                .rotationAngle  = 90.0f,
                .sweepDirection = D2D1_SWEEP_DIRECTION_CLOCKWISE,
                .arcSize        = D2D1_ARC_SIZE_SMALL
            });
            geoSink->AddLine({ tabWidth -  tabRoundRadius * 2.0f, 0.0f });

            //------------------------------------------------------------------
            // Right Top Corner
            //------------------------------------------------------------------

            geoSink->AddArc(
            {
                .point          = { tabWidth - tabRoundRadius, tabRoundRadius },
                .size           = { tabRoundRadius, tabRoundRadius },
                .rotationAngle  = 90.0f,
                .sweepDirection = D2D1_SWEEP_DIRECTION_CLOCKWISE,
                .arcSize        = D2D1_ARC_SIZE_SMALL
            });
            geoSink->AddLine({ tabWidth - tabRoundRadius, tabHeight - tabRoundRadius });

            //------------------------------------------------------------------
            // Right Bottom Corner
            //------------------------------------------------------------------

            geoSink->AddArc(
            {
                .point          = { tabWidth, tabHeight },
                .size           = { tabRoundRadius, tabRoundRadius },
                .rotationAngle  = 90.0f,
                .sweepDirection = D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
                .arcSize        = D2D1_ARC_SIZE_SMALL
            });
            geoSink->AddLine({ 0.0f, tabHeight });

            geoSink->EndFigure(D2D1_FIGURE_END_CLOSED);
        }
        THROW_IF_FAILED(geoSink->Close());
    }

    void TabGroup::OverflowButtonRes::loadMask()
    {
        TabGroup* tg = m_master;
        THROW_IF_NULL(tg);

        auto& setting = tg->appearance().tabBar.overflow.button;

        mask.loadBitmap(setting.geometry.size);
    }

    void TabGroup::OverflowButtonRes::loadPathGeo()
    {
        TabGroup* tg = m_master;
        THROW_IF_NULL(tg);

        THROW_IF_NULL(Application::g_app);

        auto factory = Application::g_app->renderer()->d2d1Factory();
        THROW_IF_FAILED(factory->CreatePathGeometry(&pathGeo));

        ComPtr<ID2D1GeometrySink> geoSink = {};
        THROW_IF_FAILED(pathGeo->Open(&geoSink));
        {
            auto& setting = tg->appearance().tabBar.overflow.icon;
            auto& points = setting.geometry.bottomTriangle.points;

            geoSink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_FILLED);

            geoSink->AddLines(points, _countof(points));

            geoSink->EndFigure(D2D1_FIGURE_END_CLOSED);
        }
        THROW_IF_FAILED(geoSink->Close());
    }

    void TabGroup::onSelectedTabChange(OptRefer<size_t> index)
    {
        onSelectedTabChangeHelper(index);

        if (f_onSelectedTabChange) f_onSelectedTabChange(this, index);
    }

    void TabGroup::onSelectedTabChangeHelper(OptRefer<size_t> index)
    {
        // This method intentionally left blank.
    }

    const TabGroup::TabImplArray& TabGroup::tabs() const
    {
        return m_tabs;
    }

    void TabGroup::insertTab(const TabArray& tabs, size_t index)
    {
        index = std::clamp(index, 0_uz, m_tabs.size());

        auto backInserter = std::inserter
        (
            m_tabs, m_tabs.begin() + index
        );
        auto makeTabImpl = [&](auto& tab)
        {
            tab.caption->m_parentTabGroup = std::dynamic_pointer_cast<TabGroup>(shared_from_this());

            registerUIEvents(tab.content);

            tab.content->setPrivateEnabled(false);
            tab.content->transform(selfCoordRect());

            TabImpl tabImpl = tab;

            tabImpl.m_overflowItem = makeUIObject<MenuItem>(nullUIObj());
            tabImpl.m_overflowItem->isInstant = false;

            return tabImpl;
        };
        std::transform(tabs.begin(), tabs.end(), backInserter, makeTabImpl);

        auto originalSelectedIndex = m_selectedTabIndex;
#define UPDATE_TAB_INDEX(Tab_Index) \
do { \
    if (Tab_Index.has_value()) \
    { \
        if (Tab_Index.value() >= index) \
        { \
            Tab_Index.value() += tabs.size(); \
        } \
    } \
} while (0)
        UPDATE_TAB_INDEX(m_hoveredTabIndex);
        UPDATE_TAB_INDEX(m_selectedTabIndex);
        UPDATE_TAB_INDEX(m_draggedTabIndex);

#undef UPDATE_TAB_INDEX

        updateAllTabs();

        if (m_selectedTabIndex != originalSelectedIndex)
        {
            onSelectedTabChange(m_selectedTabIndex);
        }
    }

    void TabGroup::appendTab(const TabArray& tabs)
    {
        insertTab(tabs, m_tabs.size());
    }

    void TabGroup::removeTab(size_t index, size_t count)
    {
        if (index < m_tabs.size())
        {
            count = std::min(count, m_tabs.size() - index);
            size_t endIndex = index + count;

            for (size_t i = index; i < endIndex; ++i)
            {
                auto& tab = m_tabs[i];

                tab.caption->m_parentTabGroup.reset();

                // Its parent could be either tab-group or overflow-menu item,
                // so using release here ensures that it is properly cleaned up.
                tab.caption->release();

                unregisterUIEvents(tab.content);

                tab.m_overflowItem->release();
            }
            m_tabs.erase(m_tabs.begin() + index, m_tabs.begin() + endIndex);

            auto originalSelectedIndex = m_selectedTabIndex;
#define UPDATE_TAB_INDEX(Tab_Index) \
do { \
    if (Tab_Index.has_value()) \
    { \
        if (Tab_Index.value() >= endIndex) \
        { \
            Tab_Index.value() -= count; \
        } \
        else if (Tab_Index.value() >= index) \
        { \
            Tab_Index.reset(); \
        } \
    } \
} while (0)
            UPDATE_TAB_INDEX(m_hoveredTabIndex);
            UPDATE_TAB_INDEX(m_selectedTabIndex);
            UPDATE_TAB_INDEX(m_draggedTabIndex);

#undef UPDATE_TAB_INDEX

            updateAllTabs();

            if (m_selectedTabIndex != originalSelectedIndex)
            {
                onSelectedTabChange(m_selectedTabIndex);
            }
        }
    }

    void TabGroup::clearAllTabs()
    {
        // There is no trivial clearing since we need to update each tab-caption.
        removeTab(0, m_tabs.size());
    }

    TabGroup::ButtonState TabGroup::getOverflowButtonState() const
    {
        if (m_isOverflowButtonPressed)
        {
            return ButtonState::Pressed;
        }
        if (m_isOverflowButtonHovered)
        {
            return ButtonState::Hovered;
        }
        return ButtonState::Idle;
    }

    const SharedPtr<PopupMenu>& TabGroup::overflowMenu() const
    {
        return m_overflowMenu;
    }

    D2D1_RECT_F TabGroup::hitTestAbsoluteRect() const
    {
        auto& geometry = appearance().tabBar.geometry;

        return math_utils::increaseTop(m_absoluteRect, -geometry.height);
    }

    D2D1_RECT_F TabGroup::draggingTestAbsoluteRect() const
    {
        auto& geometry = appearance().tabBar.geometry;

        return math_utils::rect
        (
        /* x      */ m_absoluteRect.left,
        /* y      */ m_absoluteRect.top - geometry.height,
        /* width  */ width(),
        /* height */ geometry.height
        );
    }

    D2D1_RECT_F TabGroup::tabBarAbsoluteRect() const
    {
        auto& setting = appearance().tabBar.tab.main[(size_t)TabState::Idle];
        return
        {
            m_absoluteRect.left,
            m_absoluteRect.top - setting.geometry.size.height,
            m_absoluteRect.right,
            m_absoluteRect.top + setting.geometry.roundRadius
        };
    }

    D2D1_RECT_F TabGroup::tabAbsoluteRect(OptRefer<size_t> index) const
    {
        if (index.has_value())
        {
            auto& rect = m_tabs[index.value()].m_visibleRect;
            if (rect.has_value()) return rect.value();
        }
        return math_utils::zeroRectF();
    }

    D2D1_RECT_F TabGroup::tabCaptionAbsoluteRect(OptRefer<size_t> index) const
    {
        auto state = getTabState(index);

        auto& setting = appearance().tabBar.tab.main[(size_t)state];
        auto& tabRoundRadius = setting.geometry.roundRadius;

        if (state == TabState::Selected)
        {
            D2D1_POINT_2F extension = { -tabRoundRadius, 0.0f };
            return math_utils::stretch(tabAbsoluteRect(index), extension);
        }
        else return math_utils::increaseBottom(tabAbsoluteRect(index), -tabRoundRadius);
    }

    D2D1_RECT_F TabGroup::separatorAbsoluteRect(OptRefer<size_t> index) const
    {
        auto& geometry = appearance().tabBar.separator.geometry;

        auto tabRightTop = math_utils::rightTop(tabAbsoluteRect(index));
        auto separatorLeftTop = math_utils::offset(tabRightTop, geometry.offset);

        return math_utils::rect(separatorLeftTop, geometry.size);
    }

    D2D1_RECT_F TabGroup::overflowButtonAbsoluteRect() const
    {
        auto& geometry = appearance().tabBar.overflow.button.geometry;

        auto tabBarRightTop = math_utils::rightTop(tabBarAbsoluteRect());
        auto overflowButtonLeftTop = math_utils::offset(tabBarRightTop, geometry.offset);

        return math_utils::rect(overflowButtonLeftTop, geometry.size);
    }

    OptRefer<size_t> TabGroup::selectedTabIndex() const
    {
        return m_selectedTabIndex;
    }

    void TabGroup::setSelectedTab(OptRefer<size_t> index)
    {
        if (index.has_value() && index.value() >= m_tabs.size()) return;

        auto originalSelectedIndex = m_selectedTabIndex;

        /////////////////////////////
        // Step 1 - Reset Original //
        /////////////////////////////

        if (index != m_selectedTabIndex && m_selectedTabIndex.has_value())
        {
            auto& tab = m_tabs[m_selectedTabIndex.value()];
            tab.content->setPrivateEnabled(false);
        }
        ///////////////////////////////
        // Step 2 - Swap Target Tabs //
        ///////////////////////////////

        if (index.has_value() && index.value() >= m_visibleTabCount)
        {
            if (!m_selectedTabIndex.has_value())
            {
                m_selectedTabIndex = 0;
            }
            auto& tab1 = m_tabs[index.value()];
            auto& tab2 = m_tabs[m_selectedTabIndex.value()];
            std::swap(tab1, tab2);
        }
        else m_selectedTabIndex = index;

        ////////////////////////////
        // Step 3 - Set Candidate //
        ////////////////////////////

        if (getVisibleTabCount() > 0 && m_selectedTabIndex.has_value())
        {
            auto& tab = m_tabs[m_selectedTabIndex.value()];
            tab.content->setPrivateEnabled(true);
            tab.content->transform(selfCoordRect());
        }
        else m_selectedTabIndex.reset();

        updateAllTabs();

        ///////////////////////////////
        // Step 4 - Trigger Callback //
        ///////////////////////////////

        if (m_selectedTabIndex != originalSelectedIndex)
        {
            onSelectedTabChange(m_selectedTabIndex);
        }
    }

    TabGroup::TabState TabGroup::getTabState(OptRefer<size_t> index) const
    {
        if (index.has_value())
        {
            if (index == m_selectedTabIndex)
            {
                return TabState::Selected;
            }
            if (index == m_hoveredTabIndex)
            {
                return TabState::Hovered;
            }
            // fall through to idle tab
        }
        return TabState::Idle;
    }

    size_t TabGroup::getVisibleTabCount() const
    {
        auto& geometry = appearance().tabBar.geometry;
        float maxTabLegnth = width() - geometry.rightPadding;

        float tabLength = 0.0f;
        for (size_t i = 0; i < m_tabs.size(); ++i)
        {
            auto state = getTabState(i);

            auto& setting = appearance().tabBar.tab.main;
            auto& geometry = setting[(size_t)state].geometry;

            auto& tabWidth = geometry.size.width;

            tabLength += tabWidth;
            if (tabLength > maxTabLegnth) return i;
        }
        return m_tabs.size();
    }

    size_t TabGroup::selfcoordOffsetToTabIndex(float offset) const
    {
        if (m_visibleTabCount == 0) return 0;

        float tabLength = 0.0f;
        for (size_t i = 0; i < m_visibleTabCount; ++i)
        {
            auto state = getTabState(i);

            auto& setting = appearance().tabBar.tab.main;
            auto& geometry = setting[(size_t)state].geometry;

            auto& tabWidth = geometry.size.width;

            tabLength += tabWidth;
            if (tabLength > offset) return i;
        }
        return m_visibleTabCount - 1;
    }

    void TabGroup::updateAllTabs()
    {
        m_visibleTabCount = m_tabs.size();

        auto originalViewportOffset = m_overflowMenu->viewportOffset();
        m_overflowMenu->clearAllItems();

        /////////////////////////
        // Update Visible Tabs //
        /////////////////////////
        {
            auto& geometry = appearance().tabBar.geometry;
            float maxTabLegnth = width() - geometry.rightPadding;

            float tabLength = 0.0f;
            for (size_t i = 0; i < m_tabs.size(); ++i)
            {
                auto state = getTabState(i);

                auto& setting = appearance().tabBar.tab.main;
                auto& geometry = setting[(size_t)state].geometry;

                auto& tabWidth = geometry.size.width;
                auto& tabHeight = geometry.size.height;
                auto& tabRoundRadius = geometry.roundRadius;

                float tabLength2 = tabLength + tabWidth;

                //------------------------------------------------------------------
                // Visible Tab Count
                //------------------------------------------------------------------

                if (tabLength2 > maxTabLegnth)
                {
                    m_visibleTabCount = i; break;
                }
                //------------------------------------------------------------------
                // Visible Tab Rect
                //------------------------------------------------------------------

                auto& tabRect = m_tabs[i].m_visibleRect;
                tabRect =
                {
                    m_absoluteRect.left + tabLength,
                    m_absoluteRect.top - tabHeight,
                    m_absoluteRect.left + tabLength2,
                    m_absoluteRect.top
                };
                if (state != TabState::Selected)
                {
                    tabRect.value().bottom += tabRoundRadius;
                }
                tabLength = tabLength2;

                //------------------------------------------------------------------
                // Tab Caption Rect
                //------------------------------------------------------------------

                auto& tabCaption = m_tabs[i].caption;

                tabCaption->release();
                registerUIEvents(tabCaption);

                // The tab-caption may be disabled if it was an overflow-item.
                tabCaption->setPrivateEnabled(true);

                auto captionRect = tabCaptionAbsoluteRect(i);
                captionRect = absoluteToSelfCoord(captionRect);

                tabCaption->transform(captionRect);
            }
        }
        //////////////////////////
        // Update Overflow Menu //
        //////////////////////////
        {
            auto& setting = appearance().tabBar.tab.main[(size_t)TabState::Idle];
            auto& itemSize = setting.geometry.size;

            //------------------------------------------------------------------
            // Overflow Items
            //------------------------------------------------------------------

            auto itemRect = math_utils::heightOnlyRect(itemSize.height);

            PopupMenu::ItemArray menuItems = {};
            for (size_t i = m_visibleTabCount; i < m_tabs.size(); ++i)
            {
                auto& tab = m_tabs[i];

                // Its parent could be either tab-group or overflow-menu item,
                // so using release here ensures that it is properly cleaned up.
                tab.caption->release();
                tab.m_overflowItem->setContent(tab.caption);

                // These tabs are displayed within overflow-menu.
                tab.m_visibleRect.reset();

                tab.m_overflowItem->transform(itemRect);
                tab.m_overflowItem->state = ViewItem::State::Idle;

                menuItems.push_back(tab.m_overflowItem);
            }
            m_overflowMenu->insertItem(menuItems);

            //------------------------------------------------------------------
            // Constrained Size
            //------------------------------------------------------------------

            auto& menuWidth = itemSize.width;
            float menuHeight = (float)(m_tabs.size() - m_visibleTabCount) * itemSize.height;

            // Keeps the overflow-menu within the tab-group.
            menuHeight = std::min(menuHeight, height());

            m_overflowMenu->setSize({ menuWidth, menuHeight });

            //------------------------------------------------------------------
            // Viewport Offset
            //------------------------------------------------------------------

            m_overflowMenu->setViewportOffset(originalViewportOffset);
        }
    }

    SharedPtr<Window> TabGroup::promoteTabToWindow(size_t index)
    {
        if (index >= m_tabs.size()) return nullptr;

        // We need to make a copy here, otherwise after calling removeTab,
        // its reference count may drop to zero, causing unexpected deallocation.
        auto tab = m_tabs[index];

        removeTab(index);

        // Some UI interactions of TabGroup may trigger setPrivateEnabled(false).
        tab.content->setPrivateEnabled(true);

        auto rect = math_utils::increaseTop
        (
            tab.content->absoluteRect(), -Window::nonClientAreaDefaultHeight()
        );
        auto w = makeUIObject<Window>(tab.caption->title(), rect);

        w->setContent(tab.content);

        // By default, the promoted window follows the original size constraints.
        // These can be adjusted in the subsequent f_onTriggerTabPromoting callback.

        w->minimalWidthHint = std::max(minimalWidth(), Window::nonClientAreaMinimalWidth());
        w->minimalHeightHint = minimalHeight() + Window::nonClientAreaDefaultHeight();

        w->maximalWidthHint = std::max(maximalWidth(), Window::nonClientAreaMinimalWidth());
        w->maximalHeightHint = maximalHeight() + Window::nonClientAreaDefaultHeight();

        return w;
    }

    void TabGroup::triggerTabPromoting(MouseMoveEvent& e)
    {
        THROW_IF_NULL(Application::g_app);

        if (m_draggedTabIndex.has_value() &&
            m_tabs[m_draggedTabIndex.value()].caption->promotable)
        {
            auto w = promoteTabToWindow(m_draggedTabIndex.value());

            // To maintain UI interaction coherence, we need to ensure that
            // users can directly drag the window after the promotion completes.
            // Therefore, here we:
            // 1. Move the window to ensure the cursor is within its non-client area.
            // 2. Make the window mouse-focused to prepare window dragging.
            // 3. Set a pseudo mouse-button event to trigger window dragging.

            D2D1_POINT_2F offset =
            {
                -Window::nonClientAreaMinimalWidth() * 0.5f,
                -w->nonClientAreaHeight() * 0.5f
            };
            w->setPosition(math_utils::offset(e.cursorPoint, offset));

            auto focus = Application::FocusType::Mouse;
            Application::g_app->focusUIObject(focus, w);

            MouseButtonEvent immediateMouseButton = {};
            immediateMouseButton.cursorPoint = e.cursorPoint;
            immediateMouseButton.state = { MouseButtonEvent::State::Flag::LeftDown };

            w->onMouseButton(immediateMouseButton);

            w->registerTabGroup(std::dynamic_pointer_cast<TabGroup>(shared_from_this()));

            if (f_onTriggerTabPromoting) f_onTriggerTabPromoting(this, w.get());
        }
    }

    float TabGroup::minimalWidth() const
    {
        float minWidth = appearance().tabBar.geometry.rightPadding;

        if (m_selectedTabIndex.has_value())
        {
            auto rect = tabAbsoluteRect(m_selectedTabIndex);
            minWidth += absoluteToSelfCoord(rect).right;
        }
        return minWidth;
    }

    void TabGroup::onRendererUpdateObject2DHelper(Renderer* rndr)
    {
        ResizablePanel::onRendererUpdateObject2DHelper(rndr);

        for (auto& tab : m_tabs)
        {
            if (tab.caption->isD2d1ObjectVisible())
            {
                tab.caption->onRendererUpdateObject2D(rndr);
            }
            if (tab.content->isD2d1ObjectVisible())
            {
                tab.content->onRendererUpdateObject2D(rndr);
            }
        }
    }

    void TabGroup::onRendererDrawD2d1LayerHelper(Renderer* rndr)
    {
        if (m_selectedTabIndex.has_value())
        {
            auto& tab = m_tabs[m_selectedTabIndex.value()];
            if (tab.content->isD2d1ObjectVisible())
            {
                tab.content->onRendererDrawD2d1Layer(rndr);
            }
            ///////////////////////
            // Selected-Tab Mask //
            ///////////////////////

            if (m_selectedTabIndex.value() < m_visibleTabCount)
            {
                selectedTabRes.mask.beginDraw(rndr->d2d1DeviceContext());
                {
                    auto& setting = appearance().tabBar.tab.main[(size_t)TabState::Selected];

                    resource_utils::solidColorBrush()->SetColor(setting.background.color);
                    resource_utils::solidColorBrush()->SetOpacity(setting.background.opacity);

                    rndr->d2d1DeviceContext()->FillGeometry
                    (
                    /* geometry */ selectedTabRes.pathGeo.Get(),
                    /* brush    */ resource_utils::solidColorBrush()
                    );
                }
                selectedTabRes.mask.endDraw(rndr->d2d1DeviceContext());
            }
        }
        //////////////////////////
        // Overflow-Button Mask //
        //////////////////////////

        overflowButtonRes.mask.beginDraw(rndr->d2d1DeviceContext());
        {
            auto state = getOverflowButtonState();
            auto& setting = appearance().tabBar.overflow;

            //-------------------------------------------------------------------------
            // Button
            //-------------------------------------------------------------------------

            auto& buttonBackground = setting.button.background[(size_t)state];

            resource_utils::solidColorBrush()->SetColor(buttonBackground.color);
            resource_utils::solidColorBrush()->SetOpacity(buttonBackground.opacity);

            D2D1_ROUNDED_RECT roundedRect =
            {
                math_utils::sizeOnlyRect(setting.button.geometry.size),
                setting.button.geometry.roundRadius,
                setting.button.geometry.roundRadius
            };
            rndr->d2d1DeviceContext()->FillRoundedRectangle
            (
            /* roundedRect */ roundedRect,
            /* brush       */ resource_utils::solidColorBrush()
            );
            //-------------------------------------------------------------------------
            // Icon
            //-------------------------------------------------------------------------

            auto& iconBackground = setting.icon.background[(size_t)state];

            resource_utils::solidColorBrush()->SetColor(iconBackground.color);
            resource_utils::solidColorBrush()->SetOpacity(iconBackground.opacity);

            auto& topRect = setting.icon.geometry.topRect;

            rndr->d2d1DeviceContext()->FillRectangle
            (
            /* rect  */ math_utils::rect(topRect.offset, topRect.size),
            /* brush */ resource_utils::solidColorBrush()
            );
            rndr->d2d1DeviceContext()->FillGeometry
            (
            /* geometry */ overflowButtonRes.pathGeo.Get(),
            /* brush    */ resource_utils::solidColorBrush()
            );
        }
        overflowButtonRes.mask.endDraw(rndr->d2d1DeviceContext());
    }

    void TabGroup::onRendererDrawD2d1ObjectHelper(Renderer* rndr)
    {
        ///////////////////
        // Tab-Bar Panel //
        ///////////////////
        {
            auto& setting = appearance().tabBar.tab.main[(size_t)TabState::Idle];

            resource_utils::solidColorBrush()->SetColor(setting.background.color);
            resource_utils::solidColorBrush()->SetOpacity(setting.background.opacity);

            D2D1_ROUNDED_RECT roundedRect =
            {
                tabBarAbsoluteRect(),
                setting.geometry.roundRadius,
                setting.geometry.roundRadius
            };
            rndr->d2d1DeviceContext()->FillRoundedRectangle
            (
            /* roundedRect */ roundedRect,
            /* brush       */ resource_utils::solidColorBrush()
            );
        }
        /////////////////
        // Hovered-Tab //
        /////////////////

        if (m_hoveredTabIndex.has_value())
        {
            if (m_hoveredTabIndex != m_selectedTabIndex)
            {
                auto& setting = appearance().tabBar.tab.main[(size_t)TabState::Hovered];

                resource_utils::solidColorBrush()->SetColor(setting.background.color);
                resource_utils::solidColorBrush()->SetOpacity(setting.background.opacity);

                D2D1_ROUNDED_RECT roundedRect =
                {
                    tabAbsoluteRect(m_hoveredTabIndex),
                    setting.geometry.roundRadius,
                    setting.geometry.roundRadius
                };
                rndr->d2d1DeviceContext()->FillRoundedRectangle
                (
                /* roundedRect */ roundedRect,
                /* brush       */ resource_utils::solidColorBrush()
                );
            }
        }
        /////////////////////////
        // Selected-Tab Shadow //
        /////////////////////////

        if (m_selectedTabIndex.has_value() &&
            m_selectedTabIndex.value() < m_visibleTabCount)
        {
            selectedTabRes.mask.color = appearance().tabBar.tab.activeShadowColor;

            selectedTabRes.mask.configEffectInput(resource_utils::shadowEffect());

            auto tabRect = tabAbsoluteRect(m_selectedTabIndex);
            auto shadowPosition = math_utils::leftTop(tabRect);

            rndr->d2d1DeviceContext()->DrawImage
            (
            /* effect       */ resource_utils::shadowEffect(),
            /* targetOffset */ shadowPosition
            );
        }
        ////////////////
        // Background //
        ////////////////
        {
            auto& background = appearance().background;

            resource_utils::solidColorBrush()->SetColor(background.color);
            resource_utils::solidColorBrush()->SetOpacity(background.opacity);

            ResizablePanel::drawBackground(rndr);
        }
        /////////////
        // Content //
        /////////////

        if (m_selectedTabIndex.has_value())
        {
            auto& tab = m_tabs[m_selectedTabIndex.value()];
            if (tab.content->isD2d1ObjectVisible())
            {
                tab.content->onRendererDrawD2d1Object(rndr);
            }
        }
        ///////////////////
        // Miscellaneous //
        ///////////////////
        {
            for (size_t i = 0; i < m_visibleTabCount; ++i)
            {
                if (i == m_selectedTabIndex) continue;

                //-------------------------------------------------------------------------
                // Caption
                //-------------------------------------------------------------------------
                auto& caption = m_tabs[i].caption;

                if (caption->isD2d1ObjectVisible())
                {
                    caption->onRendererDrawD2d1Object(rndr);
                }
                //-------------------------------------------------------------------------
                // Separator
                //-------------------------------------------------------------------------
                auto currState = getTabState(i);
                auto nextState = getTabState(i + 1);

                if (currState == TabState::Idle &&
                    nextState == TabState::Idle &&
                    i != m_visibleTabCount - 1)
                {
                    auto& setting = appearance().tabBar.separator;

                    resource_utils::solidColorBrush()->SetColor(setting.background.color);
                    resource_utils::solidColorBrush()->SetOpacity(setting.background.opacity);

                    rndr->d2d1DeviceContext()->FillRectangle
                    (
                    /* rect  */ separatorAbsoluteRect(i),
                    /* brush */ resource_utils::solidColorBrush()
                    );
                }
            }
            //-------------------------------------------------------------------------
            // Overflow-Button
            //-------------------------------------------------------------------------

            rndr->d2d1DeviceContext()->DrawBitmap
            (
            /* bitmap               */ overflowButtonRes.mask.data.Get(),
            /* destinationRectangle */ overflowButtonAbsoluteRect(),
            /* opacity              */ overflowButtonRes.mask.opacity,
            /* interpolationMode    */ overflowButtonRes.mask.getInterpolationMode()
            );
        }
        /////////////////////////
        // Selected-Tab Object //
        /////////////////////////

        if (m_selectedTabIndex.has_value() &&
            m_selectedTabIndex.value() < m_visibleTabCount)
        {
            //-------------------------------------------------------------------------
            // Entity
            //-------------------------------------------------------------------------
            auto& setting = appearance().tabBar.tab.main[(size_t)TabState::Selected];

            rndr->d2d1DeviceContext()->DrawBitmap
            (
            /* bitmap               */ selectedTabRes.mask.data.Get(),
            /* destinationRectangle */ tabAbsoluteRect(m_selectedTabIndex),
            /* opacity              */ setting.background.opacity,
            /* interpolationMode    */ selectedTabRes.mask.getInterpolationMode()
            );
            //-------------------------------------------------------------------------
            // Caption
            //-------------------------------------------------------------------------
            auto& caption = m_tabs[m_selectedTabIndex.value()].caption;

            if (caption->isD2d1ObjectVisible())
            {
                caption->onRendererDrawD2d1Object(rndr);
            }
        }
        ///////////////////////
        // Mask below Window //
        ///////////////////////

        if (!associatedWindow.expired())
        {
            auto targetWindow = associatedWindow.lock();
            auto targetTabGroup = targetWindow->associatedTabGroup.lock();

            if (cpp_lang_utils::isMostDerivedEqual(targetTabGroup, shared_from_this()))
            {
                auto& maskSetting = appearance().maskWhenBelowDemotingWindow;

                resource_utils::solidColorBrush()->SetColor(maskSetting.color);
                resource_utils::solidColorBrush()->SetOpacity(maskSetting.opacity);

                rndr->d2d1DeviceContext()->FillRoundedRectangle
                (
                /* roundedRect */ { m_absoluteRect, roundRadiusX, roundRadiusY },
                /* brush       */ resource_utils::solidColorBrush()
                );
            }
        }
    }

    void TabGroup::drawD2d1ObjectPosterior(Renderer* rndr)
    {
        /////////////
        // Outline //
        /////////////

        auto& stroke = appearance().stroke;

        resource_utils::solidColorBrush()->SetColor(stroke.color);
        resource_utils::solidColorBrush()->SetOpacity(stroke.opacity);

        // In general, the round-radius of the tab-group is 0, so we can draw
        // left, right and bottom lines separately to hide the topmost border.

        auto leftTop = absolutePosition();

        float selfWidth = width();
        float selfHeight = height();

        auto point00 = math_utils::offset
        (
        /* point  */ leftTop,
        /* offset */ { stroke.width * 0.5f, 0.0f }
        );
        auto point01 = math_utils::offset
        (
        /* point  */ point00,
        /* offset */ { 0.0f, selfHeight }
        );
        auto point10 = math_utils::offset
        (
        /* point  */ leftTop,
        /* offset */ { selfWidth - stroke.width * 0.5f, 0.0f }
        );
        auto point11 = math_utils::offset
        (
        /* point  */ point10,
        /* offset */ { 0.0f, selfHeight }
        );
        auto point20 = math_utils::offset
        (
        /* point  */ leftTop,
        /* offset */ { 0.0f, selfHeight - stroke.width * 0.5f }
        );
        auto point21 = math_utils::offset
        (
        /* point  */ point20,
        /* offset */ { selfWidth, 0.0f }
        );
        rndr->d2d1DeviceContext()->DrawLine
        (
        /* point0      */ point00,
        /* point1      */ point01,
        /* brush       */ resource_utils::solidColorBrush(),
        /* strokeWidth */ stroke.width
        );
        rndr->d2d1DeviceContext()->DrawLine
        (
        /* point0      */ point10,
        /* point1      */ point11,
        /* brush       */ resource_utils::solidColorBrush(),
        /* strokeWidth */ stroke.width
        );
        rndr->d2d1DeviceContext()->DrawLine
        (
        /* point0      */ point20,
        /* point1      */ point21,
        /* brush       */ resource_utils::solidColorBrush(),
        /* strokeWidth */ stroke.width
        );
        ///////////////////////
        // Content Posterior //
        ///////////////////////

        if (m_selectedTabIndex.has_value())
        {
            auto& tab = m_tabs[m_selectedTabIndex.value()];
            if (tab.content->isD2d1ObjectVisible())
            {
                tab.content->drawD2d1ObjectPosterior(rndr);
            }
        }
        ResizablePanel::drawD2d1ObjectPosterior(rndr);
    }

    bool TabGroup::isHitHelper(const Event::Point& p) const
    {
        return math_utils::isOverlapped(p, sizingFrameExtendedRect(hitTestAbsoluteRect()));
    }

    void TabGroup::onSizeHelper(SizeEvent& e)
    {
        ResizablePanel::onSizeHelper(e);

        if (getVisibleTabCount() != m_visibleTabCount) updateAllTabs();

        if (m_selectedTabIndex.has_value())
        {
            auto& tab = m_tabs[m_selectedTabIndex.value()];
            tab.content->setSize(size());
        }
    }

    void TabGroup::onChangeThemeStyleHelper(const ThemeStyle& style)
    {
        ResizablePanel::onChangeThemeStyleHelper(style);

        appearance().changeTheme(style.name);

        for (auto& tab : m_tabs)
        {
            // For those overflow items with non-expired parents,
            // the appearance is updated by their shared overflow menu;
            // others require manual updates (as they are not managed).

            if (tab.m_overflowItem->parent().expired())
            {
                tab.m_overflowItem->onChangeThemeStyle(style);
            }
        }
    }

    void TabGroup::onMouseMoveHelper(MouseMoveEvent& e)
    {
        ResizablePanel::onMouseMoveHelper(e);

        auto& p = e.cursorPoint;

        if (math_utils::isOverlapped(p, draggingTestAbsoluteRect()))
        {
            if (m_draggedTabIndex.has_value())
            {
                auto& setting = appearance().tabBar;
                auto& draggingThreshold = setting.tab.draggingThreshold;

                float draggingDelta = p.x - m_draggingPoint.x;
                if (std::abs(draggingDelta) >= draggingThreshold)
                {
                    m_isDraggingTab = true;
                }
                // During the initial dragging attempt,
                // the tab only starts moving after draggingDelta exceeds draggingThreshold.
                // In subsequent dragging operations, draggingThreshold is no longer considered,
                // and the tab should keep moving continuously.
                if (m_isDraggingTab)
                {
                    //////////////////////////////
                    // Calculate Dragging Delta //
                    //////////////////////////////

                    auto nextTabIndex = selfcoordOffsetToTabIndex(absoluteToSelfCoord(p).x);
                    if (nextTabIndex != m_draggedTabIndex)
                    {
                        auto& tabWidth = setting.tab.main[(size_t)TabState::Idle].geometry.size.width;
                        m_draggingPoint.x += tabWidth * ((float)nextTabIndex - (float)m_draggedTabIndex.value());

                        draggingDelta = p.x - m_draggingPoint.x;

                        std::swap(m_tabs[m_draggedTabIndex.value()], m_tabs[nextTabIndex]);
                        m_hoveredTabIndex = m_selectedTabIndex = m_draggedTabIndex = nextTabIndex;

                        updateAllTabs();

                        auto& tab = m_tabs[m_draggedTabIndex.value()];
                        m_draggingVisibleRect = tab.m_visibleRect.value_or(math_utils::zeroRectF());
                        m_draggingCaptionRect = tab.caption->relativeRect();
                    }
                    float maxTabLegnth = width() - setting.geometry.rightPadding;

                    auto draggingLeftmost = -m_draggingVisibleRect.left;
                    auto draggingRightmost = maxTabLegnth - m_draggingVisibleRect.right;

                    draggingDelta = std::clamp(draggingDelta, draggingLeftmost, draggingRightmost);

                    //////////////////////////////
                    // Update Tab Geometry Data //
                    //////////////////////////////

                    auto& tab = m_tabs[m_draggedTabIndex.value()];
                    tab.m_visibleRect = math_utils::offset(m_draggingVisibleRect, { draggingDelta, 0.0f });
                    tab.caption->transform(math_utils::offset(m_draggingCaptionRect, { draggingDelta, 0.0f }));
                }
            }
            else // No candidate dragged tab.
            {
                m_hoveredTabIndex.reset();

                for (size_t i = 0; i < m_visibleTabCount; ++i)
                {
                    if (math_utils::isInside(p, tabAbsoluteRect(i)))
                    {
                        m_hoveredTabIndex = i; break;
                    }
                }
                if (!math_utils::isInside(p, overflowButtonAbsoluteRect()))
                {
                    m_isOverflowButtonHovered = false;
                    m_isOverflowButtonPressed = false;
                }
                else m_isOverflowButtonHovered = true;
            }
        }
        else // Out of the dragging area.
        {
            m_hoveredTabIndex.reset();

            if (e.buttonState.leftPressed)
            {
                triggerTabPromoting(e);
            }
            // Note: The reset of tab-rect and caption-rect must be placed after
            // calling triggerTabPromoting, as promotion may update m_draggedTabIndex.
            if (m_draggedTabIndex.has_value())
            {
                auto& tab = m_tabs[m_draggedTabIndex.value()];
                tab.m_visibleRect = m_draggingVisibleRect;
                tab.caption->transform(m_draggingCaptionRect);
                m_draggedTabIndex.reset();
            }
            m_isDraggingTab = false;

            m_isOverflowButtonHovered = false;
            m_isOverflowButtonPressed = false;
        }
    }

    void TabGroup::onMouseLeaveHelper(MouseMoveEvent& e)
    {
        ResizablePanel::onMouseLeaveHelper(e);

        m_hoveredTabIndex.reset();

        if (e.buttonState.leftPressed)
        {
            triggerTabPromoting(e);
        }
        // Note: The reset of tab-rect and caption-rect must be placed after
        // calling triggerTabPromoting, as promotion may update m_draggedTabIndex.
        if (m_draggedTabIndex.has_value())
        {
            auto& tab = m_tabs[m_draggedTabIndex.value()];
            tab.m_visibleRect = m_draggingVisibleRect;
            tab.caption->transform(m_draggingCaptionRect);
            m_draggedTabIndex.reset();
        }
        m_isDraggingTab = false;

        m_isOverflowButtonHovered = false;
        m_isOverflowButtonPressed = false;
    }

    void TabGroup::onMouseButtonHelper(MouseButtonEvent& e)
    {
        ResizablePanel::onMouseButtonHelper(e);

        auto& p = e.cursorPoint;

        if (e.state.leftDown() || e.state.leftDblclk())
        {
            if (m_hoveredTabIndex.has_value())
            {
                auto& tab = m_tabs[m_hoveredTabIndex.value()];

                if (!(tab.caption->closable && tab.caption->m_isCloseButtonHover))
                {
                    if (tab.caption->draggable)
                    {
                        m_draggedTabIndex = m_hoveredTabIndex;

                        m_draggingPoint = p;
                        m_draggingVisibleRect = tab.m_visibleRect.value_or(math_utils::zeroRectF());
                        m_draggingCaptionRect = tab.caption->relativeRect();
                    }
                }
            }
            m_isOverflowButtonPressed = m_isOverflowButtonHovered;
        }
        else if (e.state.leftUp())
        {
            if (m_draggedTabIndex.has_value())
            {
                auto& tab = m_tabs[m_draggedTabIndex.value()];
                tab.m_visibleRect = m_draggingVisibleRect;
                tab.caption->transform(m_draggingCaptionRect);
                m_draggedTabIndex.reset();
            }
            m_isDraggingTab = false;

            if (m_isOverflowButtonPressed)
            {
                m_isOverflowButtonPressed = false;

                m_overflowMenu->setPosition(math_utils::offset(
                    math_utils::rightTop(m_absoluteRect),
                    { -m_overflowMenu->width(), 0.0f }));

                m_overflowMenu->setViewportOffset({ 0.0f, 0.0f });
                m_overflowMenu->setActivated(true);
            }
        }
    }
}
