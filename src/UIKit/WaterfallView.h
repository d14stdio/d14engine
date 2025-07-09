#pragma once

#include "Common/Precompile.h"

// Do NOT remove this header for code tidy
// as the template deduction relies on it.
#include "Common/CppLangUtils/PointerCompare.h"

#include "UIKit/ConstraintLayout.h"
#include "UIKit/ScrollView.h"
#include "UIKit/ViewItem.h"

namespace d14engine::uikit
{
    template<typename Item_T>
    struct WaterfallView : ScrollView
    {
        static_assert(std::is_base_of_v<ViewItem, Item_T>,
            "Item_T must inherit from d14engine::uikit::ViewItem");

        explicit WaterfallView(const D2D1_RECT_F& rect = {})
            :
            Panel(rect, resource_utils::solidColorBrush()),
            ScrollView(makeUIObject<ConstraintLayout>(), rect)
        {
            // Here left blank intentionally.
        }

        void onInitializeFinish() override
        {
            ScrollView::onInitializeFinish();

            m_layout = std::dynamic_pointer_cast<ConstraintLayout>(m_content);

            m_layout->f_onReleaseUIObject = [this]
            (Panel* p, ShrdPtrRefer<Panel> uiobj)
            {
                for (size_t i = 0; i < m_items.size(); ++i)
                {
                    auto& item = m_items[i];
                    if (cpp_lang_utils::isMostDerivedEqual(uiobj, item))
                    {
                        removeItem(i); return true;
                    }
                }
                return false;
            };
            m_layout->f_onSize = [this](Panel* p, SizeEvent& e)
            {
                // The viewport offset may be invalid after resizing.
                setViewportOffset(m_viewportOffset);
            };
        }

        ////////////////////////
        // Callback Functions //
        ////////////////////////

        //------------------------------------------------------------------
        // Public Interfaces
        //------------------------------------------------------------------
    public:
        using ItemIndexSet = std::set<size_t>;

        void onSelectChange(const ItemIndexSet& selected)
        {
            onSelectChangeHelper(selected);

            if (f_onSelectChange) f_onSelectChange(this, selected);
        }
        Function<void(WaterfallView*, const ItemIndexSet&)> f_onSelectChange = {};

        //------------------------------------------------------------------
        // Protected Helpers
        //------------------------------------------------------------------
    protected:
        virtual void onSelectChangeHelper(const ItemIndexSet& selected)
        {
            // This method intentionally left blank.
        }

        //////////////////////////
        // Graphical Components //
        //////////////////////////

        //------------------------------------------------------------------
        // Child Objects
        //------------------------------------------------------------------
    protected:
        SharedPtr<ConstraintLayout> m_layout = {};

    public:
        void updateItemConstraints()
        {
            float offset = 0.0f;
            for (auto& item : m_items)
            {
                auto itor = m_layout->findElement(item);
                if (itor.has_value())
                {
                    auto& elem = itor.value()->first;
                    auto& geoInfo = itor.value()->second;

                    geoInfo.Top.ToTop = offset;
                    m_layout->updateElement(elem, geoInfo);
                }
                offset += item->height();
            }
            m_layout->setSize(width(), offset);
        }

    public:
        using ItemArray = std::vector<SharedPtr<Item_T>>;

    protected:
        ItemArray m_items = {};

        ItemIndexSet m_selectedItemIndices = {};

    public:
        const ItemArray& items() const
        {
            return m_items;
        }
        const ItemIndexSet& selectedItemIndices() const
        {
            return m_selectedItemIndices;
        }

        virtual void insertItem(const ItemArray& items, size_t index = 0)
        {
            index = std::clamp(index, 0_uz, m_items.size());

            //------------------------------------------------------
            // Master Layout
            //------------------------------------------------------
            float height = 0.0f;
            for (auto& item : items)
            {
                height += item->height();
            }
            m_layout->setSize(width(), m_layout->height() + height);

            float offset = 0.0f;
            //------------------------------------------------------
            // Lower Items
            //------------------------------------------------------
            for (size_t i = 0; i < index; ++i)
            {
                offset += m_items[i]->height();
            }
            //------------------------------------------------------
            // Inserted Items
            //------------------------------------------------------
            for (auto& item : items)
            {
                item->setPrivateVisible(false);
                item->appEventReactability.hitTest = false;

                ConstraintLayout::GeometryInfo geoInfo = {};

                geoInfo.keepWidth = false;
                geoInfo.Left.ToLeft = 0.0f;
                geoInfo.Right.ToRight = 0.0f;
                geoInfo.Top.ToTop = offset;

                m_layout->addElement(item, geoInfo);

                offset += item->height();
            }
            //------------------------------------------------------
            // Higher Items
            //------------------------------------------------------
            for (size_t i = index; i < m_items.size(); ++i)
            {
                auto itor = m_layout->findElement(m_items[i]);
                if (itor.has_value())
                {
                    auto& elem = itor.value()->first;
                    auto& geoInfo = itor.value()->second;

                    geoInfo.Top.ToTop = offset;
                    m_layout->updateElement(elem, geoInfo);
                }
                offset += m_items[i]->height();
            }
            //------------------------------------------------------
            // Item Array
            //------------------------------------------------------

            m_items.insert(m_items.begin() + index, items.begin(), items.end());

            //------------------------------------------------------
            // Item Indices
            //------------------------------------------------------
            ItemIndexSet updatedItemIndices = {};
            for (auto& itemIndex : m_selectedItemIndices)
            {
                if (itemIndex < index)
                {
                    updatedItemIndices.insert(itemIndex);
                }
                else // needs to update
                {
                    updatedItemIndices.insert(itemIndex + items.size());
                }
            }
            m_selectedItemIndices = std::move(updatedItemIndices);

#define UPDATE_ITEM_INDEX(Item_Index) \
do { \
    if (Item_Index.has_value()) \
    { \
        auto& indexValue = Item_Index.value(); \
        if (indexValue >= index) \
        { \
            indexValue += items.size(); \
        } \
    } \
} while (0)
            UPDATE_ITEM_INDEX(m_lastHoverItemIndex);
            UPDATE_ITEM_INDEX(m_lastSelectedItemIndex);
            UPDATE_ITEM_INDEX(m_extendedSelectItemIndex);

            UPDATE_ITEM_INDEX(m_visibleItemIndexRange.index1);
            UPDATE_ITEM_INDEX(m_visibleItemIndexRange.index2);

#undef UPDATE_ITEM_INDEX

            updateVisibleItems();
        }

        virtual void removeItem(size_t index, size_t count = 1)
        {
            if (index < m_items.size())
            {
                count = std::min(count, m_items.size() - index);
                size_t endIndex = index + count;

                //------------------------------------------------------
                // Master Layout
                //------------------------------------------------------
                float height = 0.0f;
                for (size_t i = index; i < endIndex; ++i)
                {
                    height += m_items[i]->height();
                }
                m_layout->setSize(width(), m_layout->height() - height);

                float offset = 0.0f;
                //------------------------------------------------------
                // Lower Items
                //------------------------------------------------------
                for (size_t i = 0; i < index; ++i)
                {
                    offset += m_items[i]->height();
                }
                //------------------------------------------------------
                // Removed Items
                //------------------------------------------------------
                for (size_t i = index; i < endIndex; ++i)
                {
                    m_layout->removeElement(m_items[i]);
                }
                //------------------------------------------------------
                // Higher Items
                //------------------------------------------------------
                for (size_t i = endIndex; i < m_items.size(); ++i)
                {
                    auto itor = m_layout->findElement(m_items[i]);
                    if (itor.has_value())
                    {
                        auto& elem = itor.value()->first;
                        auto& geoInfo = itor.value()->second;

                        geoInfo.Top.ToTop = offset;
                        m_layout->updateElement(elem, geoInfo);
                    }
                    offset += m_items[i]->height();
                }
                //------------------------------------------------------
                // Item Array
                //------------------------------------------------------

                m_items.erase(m_items.begin() + index, m_items.begin() + endIndex);

                //------------------------------------------------------
                // Item Indices
                //------------------------------------------------------
                ItemIndexSet updatedItemIndices = {};
                for (auto& itemIndex : m_selectedItemIndices)
                {
                    if (itemIndex < index)
                    {
                        updatedItemIndices.insert(itemIndex);
                    }
                    else if (itemIndex >= endIndex)
                    {
                        updatedItemIndices.insert(itemIndex - count);
                    }
                }
                m_selectedItemIndices = std::move(updatedItemIndices);

#define UPDATE_ITEM_INDEX(Item_Index) \
do { \
    if (Item_Index.has_value()) \
    { \
        auto& indexValue = Item_Index.value(); \
        if (indexValue < index) \
        { \
            /* No change needed. */ \
        } \
        else if (indexValue >= endIndex) \
        { \
            indexValue -= count; \
        } \
        else Item_Index.reset(); \
    } \
} while (0)
                UPDATE_ITEM_INDEX(m_lastHoverItemIndex);
                UPDATE_ITEM_INDEX(m_lastSelectedItemIndex);
                UPDATE_ITEM_INDEX(m_extendedSelectItemIndex);

                UPDATE_ITEM_INDEX(m_visibleItemIndexRange.index1);
                UPDATE_ITEM_INDEX(m_visibleItemIndexRange.index2);

#undef UPDATE_ITEM_INDEX

                updateVisibleItems();
            }
        }

        virtual void clearAllItems()
        {
            m_layout->clearAllElements();
            m_layout->setSize(m_layout->width(), 0.0f);

            m_items.clear();
            m_selectedItemIndices.clear();

            m_lastHoverItemIndex.reset();
            m_lastSelectedItemIndex.reset();
            m_extendedSelectItemIndex.reset();

            m_visibleItemIndexRange.index1.reset();
            m_visibleItemIndexRange.index2.reset();
        }

        ///////////////////////
        // Interaction Logic //
        ///////////////////////

        //------------------------------------------------------------------
        // Select Mode
        //------------------------------------------------------------------
    public:
        enum class SelectMode
        {
            None, Single, Multiple, Extended
        }
        selectMode = SelectMode::Extended;

        //------------------------------------------------------------------
        // Select Trigger
        //------------------------------------------------------------------
    protected:
        Optional<size_t> m_lastHoverItemIndex = {};
        Optional<size_t> m_lastSelectedItemIndex = {};
        Optional<size_t> m_extendedSelectItemIndex = {};

        void triggerNoneSelect()
        {
            for (auto& i : m_selectedItemIndices)
            {
                m_items[i]->triggerUnchkStateTrans();
            }
            m_selectedItemIndices.clear();

            m_lastSelectedItemIndex.reset();
            m_extendedSelectItemIndex.reset();
        }

        void triggerSingleSelect(size_t itemIndex)
        {
            for (auto& i : m_selectedItemIndices)
            {
                m_items[i]->triggerUnchkStateTrans();
            }
            m_items[itemIndex]->triggerCheckStateTrans();

            m_selectedItemIndices = { itemIndex };

            m_lastSelectedItemIndex = itemIndex;
            m_extendedSelectItemIndex = itemIndex;
        }

        void triggerMultipleSelect(size_t itemIndex)
        {
            auto itor = m_selectedItemIndices.find(itemIndex);
            if (itor != m_selectedItemIndices.end())
            {
                m_items[itemIndex]->triggerUnchkStateTrans();

                m_selectedItemIndices.erase(itor);

                if (m_lastSelectedItemIndex == itemIndex)
                {
                    m_lastSelectedItemIndex.reset();
                }
                if (m_extendedSelectItemIndex == itemIndex)
                {
                    m_extendedSelectItemIndex.reset();
                }
            }
            else // select new item
            {
                m_items[itemIndex]->triggerCheckStateTrans();

                m_selectedItemIndices.insert(itemIndex);

                if (m_lastSelectedItemIndex.has_value())
                {
                    auto& item = m_items[m_lastSelectedItemIndex.value()];
                    item->triggerLeaveStateTrans();
                }
                m_lastSelectedItemIndex = itemIndex;
                m_extendedSelectItemIndex = itemIndex;
            }
        }

        void triggerShiftKeySelect(size_t itemIndex)
        {
            if (m_extendedSelectItemIndex.has_value())
            {
                auto& extIndex = m_extendedSelectItemIndex.value();
                auto range = std::minmax(itemIndex, extIndex);

                for (auto i = range.first; i <= range.second; ++i)
                {
                    m_selectedItemIndices.insert(i);

                    m_items[i]->triggerCheckStateTrans();
                    if (i != itemIndex)
                    {
                        // Highlight only the last selected item.
                        m_items[i]->triggerLeaveStateTrans();
                    }
                }
                m_lastSelectedItemIndex = itemIndex;
            }
            else triggerSingleSelect(itemIndex);
        }

        void triggerExtendedSelect(size_t itemIndex)
        {
            if (KeyboardEvent::SHIFT())
            {
                triggerShiftKeySelect(itemIndex);
            }
            else if (KeyboardEvent::CTRL())
            {
                triggerMultipleSelect(itemIndex);
            }
            else triggerSingleSelect(itemIndex);
        }

        //------------------------------------------------------------------
        // Item Index Calculators
        //------------------------------------------------------------------
    public:
        Optional<size_t> getItemIndex(ShrdPtrRefer<Item_T> item) const
        {
            for (size_t i = 0; i < m_items.size(); ++i)
            {
                if (cpp_lang_utils::isMostDerivedEqual(item, m_items[i]))
                {
                    return i;
                }
            }
            return std::nullopt;
        }

        Optional<size_t> viewportOffsetToItemIndex(float offset) const
        {
            if (offset >= 0.0f && offset <= m_layout->height())
            {
                float height = 0.0f;
                for (size_t i = 0; i < m_items.size(); ++i)
                {
                    height += m_items[i]->height();
                    if (height > offset) return i;
                }
            }
            return std::nullopt;
        }

        //------------------------------------------------------------------
        // Visible Item Index Range
        //------------------------------------------------------------------
    protected:
        struct ItemIndexRange
        {
            Optional<size_t> index1 = {}, index2 = {};
        }
        m_visibleItemIndexRange = {};

        virtual void visibleItemsUpdateFunc(bool value)
        {
            auto& range = m_visibleItemIndexRange;
            if (range.index1.has_value() && range.index2.has_value())
            {
                auto& index1 = range.index1.value();
                auto& index2 = range.index2.value();

                for (auto i = index1; i <= index2; ++i)
                {
                    auto& item = m_items[i];

                    item->setPrivateVisible(value);
                    item->appEventReactability.hitTest = value;
                }
            }
        }

    public:
        void updateVisibleItems()
        {
            visibleItemsUpdateFunc(false);

            auto& range = m_visibleItemIndexRange;
            range.index1 = viewportOffsetToItemIndex(m_viewportOffset.y);
            range.index2 = viewportOffsetToItemIndex(m_viewportOffset.y + height());

            if (range.index1.has_value() &&
               !range.index2.has_value())
            {
                range.index2 = m_items.size() - 1;
            }
            visibleItemsUpdateFunc(true);
        }

        /////////////////////////
        // Interface Overrides //
        /////////////////////////

    protected:
        //------------------------------------------------------------------
        // Panel
        //------------------------------------------------------------------

        void onGetKeyboardFocusHelper() override
        {
            ScrollView::onGetKeyboardFocusHelper();

            for (auto& i : m_selectedItemIndices)
            {
                m_items[i]->triggerGetfcStateTrans();
            }
        }

        void onLoseKeyboardFocusHelper() override
        {
            ScrollView::onLoseKeyboardFocusHelper();

            for (auto& i : m_selectedItemIndices)
            {
                m_items[i]->triggerLosfcStateTrans();
            }
        }

        void onSizeHelper(SizeEvent& e) override
        {
            auto originalViewportOffset = m_viewportOffset;

            ScrollView::onSizeHelper(e);

            m_layout->setSize(e.size.width, m_layout->height());

            // NOTE: The unchanged viewportOffset indicates
            // onViewportOffsetChange didn't be triggered above.
            // So, we must manually call updateVisibleItems
            // (As the original update resides in the callback).

            if (m_viewportOffset.x == originalViewportOffset.x &&
                m_viewportOffset.y == originalViewportOffset.y)
            {
                updateVisibleItems();
            }
        }

        void onMouseMoveHelper(MouseMoveEvent& e) override
        {
            ScrollView::onMouseMoveHelper(e);

            auto& p = e.cursorPoint;
            auto relative = absoluteToSelfCoord(p);

            // In case trigger by mistake when controlling the scroll bars.
            auto itemIndex = isControllingScrollBars() ? std::nullopt :
                viewportOffsetToItemIndex(m_viewportOffset.y + relative.y);

            if (itemIndex != m_lastHoverItemIndex)
            {
                if (itemIndex.has_value())
                {
                    auto& indexValue = itemIndex.value();
                    m_items[indexValue]->triggerEnterStateTrans();
                }
                if (m_lastHoverItemIndex.has_value())
                {
                    auto& indexValue = m_lastHoverItemIndex.value();
                    m_items[indexValue]->triggerLeaveStateTrans();
                }
            }
            m_lastHoverItemIndex = itemIndex;
        }

        void onMouseLeaveHelper(MouseMoveEvent& e) override
        {
            ScrollView::onMouseLeaveHelper(e);

            if (m_lastHoverItemIndex.has_value())
            {
                auto& indexValue = m_lastHoverItemIndex.value();
                m_items[indexValue]->triggerLeaveStateTrans();
            }
            m_lastHoverItemIndex.reset();
        }

        void onMouseButtonHelper(MouseButtonEvent& e) override
        {
            ScrollView::onMouseButtonHelper(e);

            THROW_IF_NULL(Application::g_app);

            auto& p = e.cursorPoint;
            auto relative = absoluteToSelfCoord(p);

            if (e.state.leftDown())
            {
                // In case trigger by mistake when controlling the scroll bars.
                auto itemIndex = isControllingScrollBars() ? std::nullopt :
                    viewportOffsetToItemIndex(m_viewportOffset.y + relative.y);

                if (itemIndex.has_value())
                {
                    auto& indexValue = itemIndex.value();
                    if (m_items[indexValue]->m_enabled)
                    {
                        switch (selectMode)
                        {
                        case SelectMode::None: triggerNoneSelect(); break;
                        case SelectMode::Single: triggerSingleSelect(indexValue); break;
                        case SelectMode::Multiple: triggerMultipleSelect(indexValue); break;
                        case SelectMode::Extended: triggerExtendedSelect(indexValue); break;
                        default: break;
                        }
                        onSelectChange(m_selectedItemIndices);
                    }
                }
            }
        }

        //------------------------------------------------------------------
        // Scroll View
        //------------------------------------------------------------------

        void onViewportOffsetChangeHelper(const D2D1_POINT_2F& offset) override
        {
            ScrollView::onViewportOffsetChangeHelper(offset);

            updateVisibleItems();
        }
    };
}
