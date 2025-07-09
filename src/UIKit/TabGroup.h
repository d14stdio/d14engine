#pragma once

#include "Common/Precompile.h"

#include "Common/CppLangUtils/EnableMasterPtr.h"

#include "UIKit/Appearances/TabGroup.h"
#include "UIKit/ResizablePanel.h"
#include "UIKit/ShadowMask.h"

namespace d14engine::uikit
{
    struct MenuItem;
    struct PopupMenu;
    struct TabCaption;
    struct Window;

    struct TabGroup : appearance::TabGroup, ResizablePanel
    {
        friend Window;

        explicit TabGroup(const D2D1_RECT_F& rect = {});

        virtual ~TabGroup();

        void onInitializeFinish() override;

        _D14_SET_APPEARANCE_PROPERTY(TabGroup)

        //////////////////////
        // Cached Resources //
        //////////////////////

        using MasterPtr = cpp_lang_utils::EnableMasterPtr<TabGroup>;

        struct SelectedTabRes : MasterPtr
        {
            using MasterPtr::MasterPtr;

            ShadowMask mask = {};
            void loadMask();

            ComPtr<ID2D1PathGeometry> pathGeo = {};
            void loadPathGeo();
        }
        selectedTabRes{ this };

        struct OverflowButtonRes : MasterPtr
        {
            using MasterPtr::MasterPtr;

            MaskObject mask = {};
            void loadMask();

            ComPtr<ID2D1PathGeometry> pathGeo = {};
            void loadPathGeo();
        }
        overflowButtonRes{ this };

        //////////////////////////
        // Graphical Components //
        //////////////////////////

        //------------------------------------------------------------------
        // Public Interfaces
        //------------------------------------------------------------------
    public:
        void onSelectedTabChange(OptRefer<size_t> index);

        Function<void(TabGroup*, OptRefer<size_t>)> f_onSelectedTabChange = {};

        Function<void(TabGroup*, Window*)> f_onTriggerTabPromoting = {};

        //------------------------------------------------------------------
        // Protected Helpers
        //------------------------------------------------------------------
    protected:
        virtual void onSelectedTabChangeHelper(OptRefer<size_t> index);

        //////////////////////////
        // Graphical Components //
        //////////////////////////

        //------------------------------------------------------------------
        // Tabs
        //------------------------------------------------------------------
    public:
        struct Tab
        {
            SharedPtr<TabCaption> caption = {};
            SharedPtr<Panel> content = {};
        };
        using TabArray = std::vector<Tab>;

    protected:
        struct TabImpl : Tab
        {
            friend TabGroup;

            TabImpl(const Tab& tab) : Tab(tab) { }

        private:
            Optional<D2D1_RECT_F> m_visibleRect = {};
            SharedPtr<MenuItem> m_overflowItem = {};
        };
        using TabImplArray = std::vector<TabImpl>;

        TabImplArray m_tabs = {};

    public:
        const TabImplArray& tabs() const;

        void insertTab(const TabArray& tabs, size_t index = 0);
        void appendTab(const TabArray& tabs);

        void removeTab(size_t index, size_t count = 1);
        void clearAllTabs();

        //------------------------------------------------------------------
        // Overflow Button
        //------------------------------------------------------------------
    protected:
        bool m_isOverflowButtonHovered = false;
        bool m_isOverflowButtonPressed = false;

        ButtonState getOverflowButtonState() const;

        //------------------------------------------------------------------
        // Overflow Menu
        //------------------------------------------------------------------
    protected:
        SharedPtr<PopupMenu> m_overflowMenu = {};

    public:
        ShrdPtrRefer<PopupMenu> overflowMenu() const;

        //------------------------------------------------------------------
        // Drawing Properties
        //------------------------------------------------------------------
    protected:
        D2D1_RECT_F hitTestAbsoluteRect() const;

        D2D1_RECT_F draggingTestAbsoluteRect() const;

        D2D1_RECT_F tabBarAbsoluteRect() const;

        // Returns the cached value directly.
        D2D1_RECT_F tabAbsoluteRect(OptRefer<size_t> index) const;

        D2D1_RECT_F tabCaptionAbsoluteRect(OptRefer<size_t> index) const;

        D2D1_RECT_F separatorAbsoluteRect(OptRefer<size_t> index) const;

        D2D1_RECT_F overflowButtonAbsoluteRect() const;

        ///////////////////////
        // Interaction Logic //
        ///////////////////////

        //------------------------------------------------------------------
        // Tab Updating
        //------------------------------------------------------------------
    protected:
        Optional<size_t> m_hoveredTabIndex = {};
        Optional<size_t> m_selectedTabIndex = {};

    public:
        OptRefer<size_t> selectedTabIndex() const;
        void setSelectedTab(OptRefer<size_t> index);

    protected:
        TabState getTabState(OptRefer<size_t> index) const;

        // Only the visible tabs can be displayed in the tab-bar,
        // and the overflow tabs are listed in the overflow-menu.
        size_t m_visibleTabCount = 0;

        size_t getVisibleTabCount() const;

        size_t selfcoordOffsetToTabIndex(float offset) const;

    public:
        void updateAllTabs();

        //------------------------------------------------------------------
        // Tab Dragging
        //------------------------------------------------------------------
    protected:
        Optional<size_t> m_draggedTabIndex = {};

        bool m_isDraggingTab = false;
        D2D1_POINT_2F m_draggingPoint = {};

        D2D1_RECT_F m_draggingVisibleRect = {};
        D2D1_RECT_F m_draggingCaptionRect = {};

        //------------------------------------------------------------------
        // Tab Promoting
        //------------------------------------------------------------------
    public:
        // When a window is being dragged, all of the tab-groups that have
        // been registered for the window will be associated with it, and if
        // the window is then released above any associated tab-group, its
        // caption and content will be demoted and inserted as a new tab and
        // the original window will be destroyed later.

        WeakPtr<Window> associatedWindow = {};

        SharedPtr<Window> promoteTabToWindow(size_t index);

    protected:
        void triggerTabPromoting(MouseMoveEvent& e);

        /////////////////////////
        // Interface Overrides //
        /////////////////////////

    public:
        //------------------------------------------------------------------
        // Panel
        //------------------------------------------------------------------

        float minimalWidth() const override;

    protected:
        //------------------------------------------------------------------
        // IDrawObject2D
        //------------------------------------------------------------------

        void onRendererUpdateObject2DHelper(Renderer* rndr) override;

        void onRendererDrawD2d1LayerHelper(Renderer* rndr) override;

        void onRendererDrawD2d1ObjectHelper(Renderer* rndr) override;

        void drawD2d1ObjectPosterior(Renderer* rndr) override;

        //------------------------------------------------------------------
        // Panel
        //------------------------------------------------------------------

        bool isHitHelper(const Event::Point& p) const override;

        void onSizeHelper(SizeEvent& e) override;

        void onChangeThemeStyleHelper(const ThemeStyle& style) override;

        void onMouseMoveHelper(MouseMoveEvent& e) override;

        void onMouseLeaveHelper(MouseMoveEvent& e) override;

        void onMouseButtonHelper(MouseButtonEvent& e) override;
    };
}
