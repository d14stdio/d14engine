#pragma once

#include "Common/Precompile.h"

#include "UIKit/TreeViewItem.h"
#include "UIKit/WaterfallView.h"

namespace d14engine::uikit
{
    struct TreeView : WaterfallView<TreeViewItem>
    {
        explicit TreeView(const D2D1_RECT_F& rect = {});

        void onInitializeFinish() override;

    protected:
        ItemArray m_rootItems = {};

    public:
        const ItemArray& rootItems() const;

        void insertRootItem(const ItemArray& rootItems, size_t rootIndex = 0);
        void appendRootItem(const ItemArray& rootItems);

        void removeRootItem(size_t rootIndex, size_t count = 1);
        void clearAllItems() override;

    public:
        constexpr static float g_defaultHorzIndent = 24.0f;

    protected:
        float m_baseHorzIndent = g_defaultHorzIndent;
        float m_nodeHorzIndent = g_defaultHorzIndent;

    public:
        float baseHorzIndent() const;
        void setBaseHorzIndent(float value);

        float nodeHorzIndent() const;
        void setNodeHorzIndent(float value);

    protected:
        // You can insert/erase only root-items with the master tree-view.
        // To insert/erase child-items, you should use their parent-items.
        using WaterfallView::insertItem;
        using WaterfallView::removeItem;

        void setItemIndexRangeActive(bool value) override;

        Optional<size_t> getRootItemGlobalIndex(
            size_t rootIndex, // startIndex in global field
            Optional<size_t> startIndex = std::nullopt) const;
    };
}
