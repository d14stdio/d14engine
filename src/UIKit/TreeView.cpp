#include "Common/Precompile.h"

#include "UIKit/TreeView.h"

namespace d14engine::uikit
{
    TreeView::TreeView(const D2D1_RECT_F& rect)
        :
        Panel(rect, resource_utils::solidColorBrush()),
        WaterfallView(rect) { }

    void TreeView::onInitializeFinish()
    {
        WaterfallView::onInitializeFinish();

        m_layout->f_onReleaseUIObject = [this](Panel* p, ShrdPtrRefer<Panel> uiobj)
        {
            auto itemobj = std::static_pointer_cast<TreeViewItem>(uiobj);
            if (itemobj)
            {
                size_t index = 0;
                if (itemobj->parentItem().expired())
                {
                    for (auto& item : m_rootItems)
                    {
                        if (cpp_lang_utils::isMostDerivedEqual(uiobj, item))
                        {
                            removeRootItem(index);
                            return true;
                        }
                        ++index;
                    }
                }
                else // managed by another item
                {
                    auto itemobjParent = itemobj->parentItem().lock();
                    for (auto& item : itemobjParent->childItems())
                    {
                        if (cpp_lang_utils::isMostDerivedEqual(uiobj, item->ptr))
                        {
                            itemobjParent->removeItem(index);
                            return true;
                        }
                        ++index;
                    }
                }
            }
            return false;
        };
    }

    const TreeView::ItemArray& TreeView::rootItems() const
    {
        return m_rootItems;
    }

    void TreeView::insertRootItem(const ItemArray& rootItems, size_t rootIndex)
    {
        // "index == m_rootItems.size()" ---> append.
        rootIndex = std::clamp(rootIndex, 0_uz, m_rootItems.size());

        auto insertIndex = getRootItemGlobalIndex(rootIndex);
        if (insertIndex.has_value())
        {
            insertItem(getExpandedTreeViewItems(rootItems), insertIndex.value());

            for (auto& rootItem : rootItems)
            {
                rootItem->m_parentView = std::dynamic_pointer_cast<TreeView>(shared_from_this());
                rootItem->m_nodeLevel = 0;
                rootItem->m_parentItem.reset();
                rootItem->updateSelfContentHorzIndent();
                rootItem->updateChildMiscellaneousFields();
            }
            m_rootItems.insert(m_rootItems.begin() + rootIndex, rootItems.begin(), rootItems.end());
        }
    }

    void TreeView::appendRootItem(const ItemArray& rootItems)
    {
        insertRootItem(rootItems, m_rootItems.size());
    }

    void TreeView::removeRootItem(size_t rootIndex, size_t count)
    {
        if (rootIndex < m_rootItems.size() && count > 0)
        {
            count = std::min(count, m_rootItems.size() - rootIndex);
            size_t endRootIndex = rootIndex + count;

            auto removeStartIndex = getRootItemGlobalIndex(rootIndex);
            auto removeLastIndex = getRootItemGlobalIndex(count - 1, removeStartIndex);

            if (removeStartIndex.has_value() && removeLastIndex.has_value())
            {
                size_t removeCount = removeLastIndex.value() - removeStartIndex.value() + 1;
                removeItem(
                    removeStartIndex.value(),
                    removeCount + m_items[removeLastIndex.value()]->getExpandedChildCount());

                auto baseItor = std::next(m_rootItems.begin(), rootIndex);
                for (size_t i = 0; i < count; ++i)
                {
                    (*baseItor)->m_parentView.reset();
                    (*baseItor)->m_nodeLevel = 0;
                    (*baseItor)->m_parentItem.reset();
                    (*baseItor)->updateSelfContentHorzIndent();
                    (*baseItor)->updateChildMiscellaneousFields();
                }
                m_rootItems.erase(m_rootItems.begin() + rootIndex, m_rootItems.begin() + endRootIndex);
            }
        }
    }

    void TreeView::clearAllItems()
    {
        for (auto& item : m_rootItems)
        {
            item->m_parentView.reset();
            item->m_nodeLevel = 0;
            item->m_parentItem.reset();
            item->updateSelfContentHorzIndent();
            item->updateChildMiscellaneousFields();
        }
        WaterfallView::clearAllItems();
    }

    float TreeView::baseHorzIndent() const
    {
        return m_baseHorzIndent;
    }

    void TreeView::setBaseHorzIndent(float value)
    {
        m_baseHorzIndent = value;
        for (auto& item : m_items) item->updateContentHorzIndent();
    }

    float TreeView::nodeHorzIndent() const
    {
        return m_nodeHorzIndent;
    }

    void TreeView::setNodeHorzIndent(float value)
    {
        m_nodeHorzIndent = value;
        for (auto& item : m_items) item->updateContentHorzIndent();
    }

    void TreeView::visibleItemsUpdateFunc(bool value)
    {
        auto& range = m_visibleItemIndexRange;
        if (range.index1.has_value() && range.index2.has_value())
        {
            if (value)
            {
                for (size_t i = range.index1.value(); i <= range.index2.value(); ++i)
                {
                    auto value = m_items[i]->m_stateDetail.ancestorUnfolded();

                    // When folding a tree view item, we set its height to
                    // zero to hide the content, but its rect, however, is
                    // still within the scope of the visible area, so we must
                    // make sure the item is not belonging to any folded item
                    // before showing it.

                    m_items[i]->setPrivateVisible(value);
                    m_items[i]->appEventReactability.hitTest = value;
                }
            }
            else // It is always safe to deactivate an item.
            {
                for (size_t i = range.index1.value(); i <= range.index2.value(); ++i)
                {
                     m_items[i]->setPrivateVisible(false);
                     m_items[i]->appEventReactability.hitTest = false;
                }
            }
        }
    }

    Optional<size_t> TreeView::getRootItemGlobalIndex(size_t rootIndex, Optional<size_t> startIndex) const
    {
        auto globalIndex = startIndex.has_value() ? startIndex.value() : 0;
        if (m_items.empty())
        {
            if (globalIndex == 0) return 0;
            else return std::nullopt;
        }
        for (size_t i = 0; i < rootIndex; ++i)
        {
            if (globalIndex >= m_items.size()) return std::nullopt;
            globalIndex += m_items[globalIndex]->getExpandedChildCount() + 1;
        }
#pragma warning(push)
#pragma warning(disable : 26816)
        // Warning C26816: The pointer points to memory allocated on the stack (ES.65)
        // The result returned here is a copy. Not sure why MSVC gives this warning.
        return globalIndex;
#pragma warning(pop)
    }
}
