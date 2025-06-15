#include "Common/Precompile.h"

#include "UIKit/TreeViewItem.h"

#include "Common/CppLangUtils/PointerCompare.h"
#include "Common/DirectXError.h"
#include "Common/MathUtils/2D.h"

#include "Renderer/Renderer.h"

#include "UIKit/Application.h"
#include "UIKit/IconLabel.h"
#include "UIKit/ResourceUtils.h"
#include "UIKit/TreeView.h"

using namespace d14engine::renderer;

namespace d14engine::uikit
{
    TreeViewItem::TreeViewItem(
        ShrdPtrRefer<Panel> content,
        const D2D1_RECT_F& rect)
        :
        ViewItem(content, rect)
    {
        StatefulObject::m_state = { UNFOLDED };
        StatefulObject::m_stateDetail.flag = UNFOLDED;
        StatefulObject::m_stateDetail.ancestorFlag = UNFOLDED;

        m_layout = makeUIObject<ConstraintLayout>();
    }

    TreeViewItem::TreeViewItem(WstrRefer text, const D2D1_RECT_F& rect)
        : TreeViewItem(IconLabel::compactLayout(text), rect) { }


    void TreeViewItem::onInitializeFinish()
    {
        ViewItem::onInitializeFinish();

        loadArrowIconStrokeStyle();

        addUIObject(m_layout);

        m_layout->transform(selfCoordRect());

        if (m_content != nullptr)
        {
            ConstraintLayout::GeometryInfo info = {};

            info.keepWidth = info.keepHeight = false;
            info.Left.ToLeft = info.Right.ToRight = 0.0f;
            info.Top.ToTop = info.Bottom.ToBottom = 0.0f;

            m_layout->addElement(m_content, info);

            removeUIObject(m_content);
        }
        m_content = m_layout;
    }

    void TreeViewItem::loadArrowIconStrokeStyle()
    {
        THROW_IF_NULL(Application::g_app);

        auto factory = Application::g_app->renderer()->d2d1Factory();

        auto prop = D2D1::StrokeStyleProperties
        (
        /* startCap */ D2D1_CAP_STYLE_ROUND,
        /* endCap   */ D2D1_CAP_STYLE_ROUND,
        /* dashCap  */ D2D1_CAP_STYLE_ROUND
        );
        auto& style = arrowIcon.strokeStyle;

        THROW_IF_FAILED(factory->CreateStrokeStyle
        (
        /* strokeStyleProperties */ prop,
        /* dashes                */ nullptr,
        /* dashesCount           */ 0,
        /* strokeStyle           */ &style
        ));
    }

    void TreeViewItem::setEnabled(bool value)
    {
        Panel::setEnabled(value);

        if (!m_layout->children().empty())
        {
            (*m_layout->children().begin())->setEnabled(value);
        }
    }

    WeakPtr<Panel> TreeViewItem::content() const
    {
        if (m_layout->children().empty()) return {};
        else return *m_layout->children().begin();
    }

    void TreeViewItem::setContent(ShrdPtrRefer<Panel> content)
    {
        auto originalContent = TreeViewItem::content().lock();
        if (content && !cpp_lang_utils::isMostDerivedEqual(content, originalContent))
        {
            m_layout->removeElement(originalContent);

            ConstraintLayout::GeometryInfo geoInfo = {};

            geoInfo.keepWidth = geoInfo.keepHeight = false;
            geoInfo.Left.ToLeft = geoInfo.Right.ToRight = 0.0f;
            geoInfo.Top.ToTop = geoInfo.Bottom.ToBottom = 0.0f;

            m_layout->addElement(content, geoInfo);
        }
    }

    float TreeViewItem::ChildItemImpl::unfoldedHeight() const
    {
        return m_unfoldedHeight;
    }

    void TreeViewItem::ChildItemImpl::setUnfoldedHeight(float value)
    {
        m_unfoldedHeight = value;

        if (ptr->m_stateDetail.ancestorUnfolded())
        {
            ptr->setSize(ptr->width(), m_unfoldedHeight);
        }
    }

    const WeakPtr<TreeView>& TreeViewItem::parentView() const
    {
        return m_parentView;
    }

    void TreeViewItem::updateMasterViewConstraints()
    {
        if (!m_parentView.expired())
        {
            m_parentView.lock()->updateItemConstraints();
        }
    }

    size_t TreeViewItem::nodeLevel() const
    {
        return m_nodeLevel;
    }

    const WeakPtr<TreeViewItem>& TreeViewItem::parentItem() const
    {
        return m_parentItem;
    }

    const TreeViewItem::ChildItemImplArray& TreeViewItem::childrenItems() const
    {
        return m_childrenItems;
    }

    void TreeViewItem::insertItem(const ChildItemArray& items, size_t index)
    {
        index = std::clamp(index, 0_uz, m_childrenItems.size());

        // Updating parentView requires the global insertion index:
        //
        // 1. Insert after parentItem (this item) if m_childrenItems is empty.
        // 2. Insert before m_childrenItems[index]->ptr otherwise.
        //
        // Note that we need to compute the global insertion index in advance,
        // because it depends on m_childrenItems (which will be modified).

        auto parentItem = std::static_pointer_cast<TreeViewItem>(shared_from_this());

        Optional<size_t> insertIndex = {};
        if (!m_parentView.expired())
        {
            auto parentView = m_parentView.lock();
            if (m_childrenItems.empty())
            {
                insertIndex = parentView->getItemIndex(parentItem);

                // The insertion occurs after parentItem,
                // so we need to increment insertIndex by 1.
                if (insertIndex.has_value()) insertIndex.value() += 1;
            }
            else // follows scenario 2
            {
                auto& childItem = m_childrenItems[index]->ptr;
                insertIndex = parentView->getItemIndex(childItem);
            }
        }
        // We must update parentItem first before updating parentView,
        // because the activity of the items depends on the state of parentItem.

        ////////////////////////
        // Update Parent Item //
        ////////////////////////

        for (auto& item : items)
        {
            item->m_parentItem = parentItem;
            item->updateMiscellaneousFields();

            if (item->m_stateDetail.ancestorFolded())
            {
                item->setSize(item->width(), 0.0f);
                item->notifyHideChildrenItems();
            }
        }
        auto backInserter = std::inserter
        (
            m_childrenItems, m_childrenItems.begin() + index
        );
        auto makeItemImpl = [](auto& item)
        {
            auto itemImpl = std::make_shared<ChildItemImpl>(item);
            item->m_itemImplPtr = itemImpl; return itemImpl;
        };
        std::transform(items.begin(), items.end(), backInserter, makeItemImpl);

        ////////////////////////
        // Update Parent View //
        ////////////////////////

        if (!m_parentView.expired())
        {
            auto parentView = m_parentView.lock();
            auto rawParentView = (TreeView::WaterfallView*)parentView.get();

            if (insertIndex.has_value())
            {
                auto insertItems = getExpandedTreeViewItems(items);
                rawParentView->insertItem(insertItems, insertIndex.value());
            }
        }
    }

    void TreeViewItem::appendItem(const ChildItemArray& items)
    {
        insertItem(items, m_childrenItems.size());
    }

    void TreeViewItem::removeItem(size_t index, size_t count)
    {
        index = std::clamp(index, 0_uz, m_childrenItems.size());
        count = std::min(count, m_childrenItems.size() - index);
        size_t endIndex = index + count;

        // Refer to the comments in insertItem above;
        // we also need to compute the global index in advance.

        Optional<size_t> removeIndex = {}; size_t removeCount = 0;
        if (!m_parentView.expired())
        {
            auto parentView = m_parentView.lock();

            auto& childItem = m_childrenItems[index]->ptr;
            removeIndex = parentView->getItemIndex(childItem);

            for (size_t i = index; i < endIndex; ++i)
            {
                auto& childItem = m_childrenItems[i]->ptr;
                // We need to add 1 for childItem itself.
                removeCount += (childItem->getExpandedChildrenCount() + 1);
            }
        }
        // We must update parentItem first before updating parentView,
        // because the activity of the items depends on the state of parentItem.

        ////////////////////////
        // Update Parent Item //
        ////////////////////////

        for (size_t i = index; i < endIndex; ++i)
        {
            auto& childItem = m_childrenItems[i]->ptr;

            childItem->m_parentItem.reset();
            childItem->m_itemImplPtr.reset();
            childItem->updateMiscellaneousFields();
        }
        auto childItor = m_childrenItems.begin() + index;
        m_childrenItems.erase(childItor, childItor + count);

        ////////////////////////
        // Update Parent View //
        ////////////////////////

        if (!m_parentView.expired())
        {
            auto parentView = m_parentView.lock();
            auto rawParentView = (TreeView::WaterfallView*)parentView.get();

            if (removeIndex.has_value())
            {
                rawParentView->removeItem(removeIndex.value(), removeCount);
            }
        }
    }

    void TreeViewItem::clearAllItems()
    {
        // There is no trivial clearing since we do not know their indices in parentView.
        removeItem(0, m_childrenItems.size());
    }

    WeakPtrRefer<TreeViewItem::ChildItemImpl> TreeViewItem::itemImplPtr() const
    {
        return m_itemImplPtr;
    }

    void TreeViewItem::fold()
    {
        notifyHideChildrenItems();
        updateMasterViewConstraints();
    }

    void TreeViewItem::notifyHideChildrenItems()
    {
        for (auto& item : m_childrenItems)
        {
            item->ptr->setSize(item->ptr->width(), 0.0f);
            item->ptr->m_stateDetail.ancestorFlag = FOLDED;

            item->ptr->notifyHideChildrenItems();
        }
    }

    void TreeViewItem::unfold()
    {
        notifyShowChildrenItems();
        updateMasterViewConstraints();
    }

    void TreeViewItem::notifyShowChildrenItems()
    {
        for (auto& item : m_childrenItems)
        {
            item->ptr->setSize(item->ptr->width(), item->m_unfoldedHeight);
            item->ptr->m_stateDetail.ancestorFlag = UNFOLDED;

            if (item->ptr->m_stateDetail.folded())
            {
                item->ptr->notifyHideChildrenItems();
            }
            else // expanded item encountered
            {
                item->ptr->notifyShowChildrenItems();
            }
        }
    }

    size_t TreeViewItem::getExpandedChildrenCount() const
    {
        return getExpandedTreeViewItemCount(m_childrenItems);
    }

    size_t getExpandedTreeViewItemCount(const TreeViewItem::ChildItemArray& items)
    {
        size_t count = items.size();
        for (auto& item : items)
        {
            count += item->getExpandedChildrenCount();
        }
        return count;
    }

    size_t getExpandedTreeViewItemCount(const TreeViewItem::ChildItemImplArray& items)
    {
        size_t count = items.size();
        for (auto& item : items)
        {
            count += item->ptr->getExpandedChildrenCount();
        }
        return count;
    }

    TreeViewItem::ChildItemArray TreeViewItem::getExpandedChildrenItems() const
    {
        return getExpandedTreeViewItems(m_childrenItems);
    }

    TreeViewItem::ChildItemArray getExpandedTreeViewItems(const TreeViewItem::ChildItemArray& items)
    {
        TreeViewItem::ChildItemArray expandedItems = {};
        for (auto& item : items)
        {
            expandedItems.push_back(item);

            auto children = item->getExpandedChildrenItems();
            expandedItems.insert(expandedItems.end(), children.begin(), children.end());
        }
        return expandedItems;
    }

    TreeViewItem::ChildItemArray getExpandedTreeViewItems(const TreeViewItem::ChildItemImplArray& items)
    {
        TreeViewItem::ChildItemArray expandedItems = {};
        for (auto& item : items)
        {
            expandedItems.push_back(item->ptr);

            auto children = item->ptr->getExpandedChildrenItems();
            expandedItems.insert(expandedItems.end(), children.begin(), children.end());
        }
        return expandedItems;
    }

    void TreeViewItem::updateContentHorzIndent()
    {
        updateSelfContentHorzIndent();
        updateChildrenContentHorzIndents();
    }

    void TreeViewItem::updateSelfContentHorzIndent()
    {
        auto elemItor = m_layout->findElement(content().lock());
        if (elemItor.has_value())
        {
            auto& elem = elemItor.value()->first;
            auto& geoInfo = elemItor.value()->second;

            if (!m_parentView.expired())
            {
                auto parentView = m_parentView.lock();

                geoInfo.Left.ToLeft =
                    parentView->baseHorzIndent() +
                    parentView->nodeHorzIndent() * m_nodeLevel;
            }
            else geoInfo.Left.ToLeft = 0.0f;

            m_layout->updateElement(elem, geoInfo);
        }
    }

    void TreeViewItem::updateChildrenContentHorzIndents()
    {
        for (auto& item : m_childrenItems)
        {
            item->ptr->updateContentHorzIndent();
        }
    }

    void TreeViewItem::updateMiscellaneousFields()
    {
        updateSelfMiscellaneousFields();
        updateChildrenMiscellaneousFields();
    }

    void TreeViewItem::updateSelfMiscellaneousFields()
    {
        if (m_parentItem.expired())
        {
            m_stateDetail.ancestorFlag = UNFOLDED;

            m_nodeLevel = 0;
            m_parentView.reset();
        }
        else // parent available
        {
            auto parentItem = m_parentItem.lock();

            if (parentItem->m_stateDetail.unfolded() &&
                parentItem->m_stateDetail.ancestorUnfolded())
            {
                m_stateDetail.ancestorFlag = UNFOLDED;
            }
            else m_stateDetail.ancestorFlag = FOLDED;

            m_nodeLevel = parentItem->m_nodeLevel + 1;
            m_parentView = parentItem->m_parentView;
        }
        updateSelfContentHorzIndent();
    }

    void TreeViewItem::updateChildrenMiscellaneousFields()
    {
        for (auto& item : m_childrenItems)
        {
            item->ptr->updateMiscellaneousFields();
        }
    }

    void TreeViewItem::setFolded(StatefulObject::State::Flag flag)
    {
        StatefulObject::m_state.flag = flag;

        StatefulObject::Event soe = {};
        soe.flag = StatefulObject::m_state.flag;
        soe.ancestorFlag = m_stateDetail.ancestorFlag;

        if (soe != m_stateDetail)
        {
            m_stateDetail = soe;
            onStateChange(m_stateDetail);
        }
    }

    void TreeViewItem::onRendererDrawD2d1ObjectHelper(Renderer* rndr)
    {
        ViewItem::onRendererDrawD2d1ObjectHelper(rndr);

        ///////////////////////
        // Fold/Unfold Arrow //
        ///////////////////////

        if (!m_childrenItems.empty() && !m_parentView.expired())
        {
            auto& arrowSetting = appearance().arrow;
            auto& arrowGeometry = arrowSetting.geometry[StatefulObject::m_state.index()];
            auto& arrowBackground = m_enabled ? arrowSetting.background : arrowSetting.secondaryBackground;

            resource_utils::solidColorBrush()->SetColor(arrowBackground.color);
            resource_utils::solidColorBrush()->SetOpacity(arrowBackground.opacity);

            auto offset = m_nodeLevel * m_parentView.lock()->nodeHorzIndent();
            auto arrowLeftTop = math_utils::offset(absolutePosition(), { offset, 0.0f });

            rndr->d2d1DeviceContext()->DrawLine
            (
            /* point0      */ math_utils::offset(arrowLeftTop, arrowGeometry.line0.point0),
            /* point1      */ math_utils::offset(arrowLeftTop, arrowGeometry.line0.point1),
            /* brush       */ resource_utils::solidColorBrush(),
            /* strokeWidth */ arrowSetting.strokeWidth, arrowIcon.strokeStyle.Get()
            );

            rndr->d2d1DeviceContext()->DrawLine
            (
            /* point0      */ math_utils::offset(arrowLeftTop, arrowGeometry.line1.point0),
            /* point1      */ math_utils::offset(arrowLeftTop, arrowGeometry.line1.point1),
            /* brush       */ resource_utils::solidColorBrush(),
            /* strokeWidth */ arrowSetting.strokeWidth, arrowIcon.strokeStyle.Get()
            );
        }
    }

    void TreeViewItem::onSizeHelper(SizeEvent& e)
    {
        ViewItem::onSizeHelper(e);

        m_layout->transform(selfCoordRect());
    }

    void TreeViewItem::onChangeThemeStyleHelper(const ThemeStyle& style)
    {
        ViewItem::onChangeThemeStyleHelper(style);

        appearance().changeTheme(style.name);
    }

    void TreeViewItem::onMouseButtonHelper(MouseButtonEvent& e)
    {
        ViewItem::onMouseButtonHelper(e);

        bool triggerFoldUnfold = false;

        //-------------------------------------------------------
        // Trigger 1: double-click on the item.
        //-------------------------------------------------------
        if (e.state.leftDblclk())
        {
            triggerFoldUnfold = true;
        }
        //-------------------------------------------------------
        // Trigger 2: left-press on the arrow.
        //-------------------------------------------------------
        else if (e.state.leftDown())
        {
            float arrowLeft = 0.0f, arrowRight = 0.0f;
            if (!m_parentView.expired())
            {
                auto parentView = m_parentView.lock();

                arrowLeft = m_nodeLevel * parentView->nodeHorzIndent();
                arrowRight = arrowLeft + parentView->baseHorzIndent();
            }
            else // use default indent
            {
                arrowLeft = m_nodeLevel * TreeView::g_defaultHorzIndent;
                arrowRight = arrowLeft + TreeView::g_defaultHorzIndent;
            }
            auto selfCoord = absoluteToSelfCoord(e.cursorPoint);

            triggerFoldUnfold = selfCoord.x > arrowLeft && selfCoord.x < arrowRight;
        }
        if (triggerFoldUnfold)
        {
            setFolded(m_stateDetail.folded() ? UNFOLDED : FOLDED);
        }
    }

    void TreeViewItem::onStateChangeHelper(StatefulObject::Event& e)
    {
        StatefulObject::onStateChangeHelper(e);

        if (e.folded()) fold(); else unfold();

        if (!m_parentView.expired())
        {
            auto parentView = m_parentView.lock();
            parentView->updateItemIndexRangeActivity();
        }
    }
}
