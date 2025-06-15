#include "Common/Precompile.h"

#include "UIKit/ListView.h"

namespace d14engine::uikit
{
    ListView::ListView(const D2D1_RECT_F& rect)
        :
        Panel(rect, resource_utils::solidColorBrush()),
        WaterfallView(rect) { }

    void ListView::insertItem(const ItemArray& items, size_t index)
    {
        for (auto& item : items)
        {
            item->m_parentView = std::dynamic_pointer_cast<ListView>(shared_from_this());
        }
        WaterfallView::insertItem(items, index);
    }

    void ListView::appendItem(const ItemArray& items)
    {
        insertItem(items, m_items.size());
    }

    void ListView::removeItem(size_t index, size_t count)
    {
        if (index < m_items.size() && count > 0)
        {
            count = std::min(count, m_items.size() - index);
            size_t endIndex = index + count - 1;

            for (size_t i = index; i < endIndex; ++i)
            {
                m_items[i]->m_parentView.reset();
            }
        }
        WaterfallView::removeItem(index, count);
    }

    void ListView::clearAllItems()
    {
        for (auto& item : m_items)
        {
            item->m_parentView.reset();
        }
        WaterfallView::clearAllItems();
    }
}
