#pragma once

#include "Common/Precompile.h"

#include "UIKit/Appearances/TreeViewItem.h"
#include "UIKit/StatefulObject.h"
#include "UIKit/ViewItem.h"

namespace d14engine::uikit
{
    struct ConstraintLayout;
    struct TreeView;

    struct TreeViewItem
        :
        appearance::TreeViewItem, ViewItem,
        StatefulObject<TreeViewItemState, TreeViewItemStateChangeEvent>
    {
        friend TreeView;

        TreeViewItem(ShrdPtrRefer<Panel> content, const D2D1_RECT_F& rect = {});

        TreeViewItem(WstrRefer text = L"ViewItem", const D2D1_RECT_F& rect = {});

        using ChildItemArray = std::vector<SharedPtr<TreeViewItem>>;

        void onInitializeFinish() override;

        struct ArrowIcon
        {
            ComPtr<ID2D1StrokeStyle> strokeStyle = {};
        }
        arrowIcon = {};

        void loadArrowIconStrokeStyle();

        _D14_SET_APPEARANCE_PROPERTY(TreeViewItem)

        void setEnabled(bool value) override;

    protected:
        SharedPtr<ConstraintLayout> m_layout = {};

    public:
        template<typename T = Panel>
        WeakPtr<T> getContent() const
        {
            if constexpr (std::is_same_v<T, Panel>) return content();
            else return std::dynamic_pointer_cast<T>(content().lock());
        }
        WeakPtr<Panel> content() const;
        void setContent(ShrdPtrRefer<Panel> content);

    protected:
        WeakPtr<TreeView> m_parentView = {};

        // For example:
        //
        // Root---Child_0---Child_1--Child_2
        //           |         |
        //           |         \---Child_10
        //           |
        //           \---Child_00---Child_01
        //
        // where the node levels are:
        //
        // [0, parent       ] Root
        // [1, children     ] Child_0, Child_1, Child_2
        // [2, grandchildren] Child_00, Child_01, Child_10
        size_t m_nodeLevel = 0;

        WeakPtr<TreeViewItem> m_parentItem = {};

        struct ChildItemImpl
        {
            friend TreeViewItem;

            ChildItemImpl(ShrdPtrRefer<TreeViewItem> rhs)
                : ptr(rhs), m_unfoldedHeight(rhs->height()) { }

            SharedPtr<TreeViewItem> ptr = {};

        private:
            // Since the height of a folded item will be set to 0, we need
            // this backup to help restore the height after the item unfolded.
            float m_unfoldedHeight = {};

        public:
            float unfoldedHeight() const;

            // Note this method only update the item and the backup, so you
            // need to call updateMasterViewConstraints later to update the
            // appearance immediately.
            //
            // We do not call updateMasterViewConstraints in this method
            // since it can cause unnecessary performance loss when the
            // setter is called more than once, so basically you should
            // iterate all the candidate items firstly to set their heights
            // and then call updateMasterViewConstraints finally.
            void setUnfoldedHeight(float value);
        };
        using ChildItemImplArray = std::vector<SharedPtr<ChildItemImpl>>;

        ChildItemImplArray m_childItems = {};

        // Points to ChildItemImpl maintained in ChildItemImplArray of the
        // parent item (however, always equals nullptr for any root-item).
        WeakPtr<ChildItemImpl> m_itemImplPtr = {};

    public:
        const WeakPtr<TreeView>& parentView() const;

        void updateMasterViewConstraints();

        size_t nodeLevel() const;

        const WeakPtr<TreeViewItem>& parentItem() const;

        const ChildItemImplArray& childItems() const;

        void insertItem(const ChildItemArray& items, size_t index = 0);
        void appendItem(const ChildItemArray& items);

        void removeItem(size_t index, size_t count = 1);
        void clearAllItems();

        WeakPtrRefer<ChildItemImpl> itemImplPtr() const;

    protected:
        void fold(); void notifyHideChildItems();
        void unfold(); void notifyShowChildItems();

        // Convert item-tree to item-list:
        //
        // Root---Child_0---Child_1--Child_2
        //           |         |
        //           |         \---Child_10
        //           |
        //           \---Child_00---Child_01
        //
        // will be expanded to:
        //
        // Root---Child_0---Child_00---Child_01---Child_1---Child_10---Child_2

        size_t getExpandedChildCount() const;

        friend size_t getExpandedTreeViewItemCount(const ChildItemArray& items);
        friend size_t getExpandedTreeViewItemCount(const ChildItemImplArray& items);

        ChildItemArray getExpandedChildItems() const;

        friend ChildItemArray getExpandedTreeViewItems(const ChildItemArray& items);
        friend ChildItemArray getExpandedTreeViewItems(const ChildItemImplArray& items);

        void updateContentHorzIndent();
        void updateSelfContentHorzIndent();
        void updateChildContentHorzIndents();

        void updateMiscellaneousFields();
        void updateSelfMiscellaneousFields();
        void updateChildMiscellaneousFields();

    public:
        constexpr static auto FOLDED = StatefulObject::State::Flag::Folded;
        constexpr static auto UNFOLDED = StatefulObject::State::Flag::Unfolded;

        void setFolded(StatefulObject::State::Flag flag);

    protected:
        // IDrawObject2D
        void onRendererDrawD2d1ObjectHelper(renderer::Renderer* rndr) override;

        // Panel
        void onSizeHelper(SizeEvent& e) override;

        void onChangeThemeStyleHelper(const ThemeStyle& style) override;

        void onMouseButtonHelper(MouseButtonEvent& e) override;

        // StatefulObject
        void onStateChangeHelper(StatefulObject::Event& e) override;
    };
}
