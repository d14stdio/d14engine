﻿#include "Common/Precompile.h"

#include "UIKit/ScrollView.h"

#include "Common/CppLangUtils/PointerCompare.h"
#include "Common/MathUtils/2D.h"
#include "Common/RuntimeError.h"

#include "Renderer/Renderer.h"

#include "UIKit/Application.h"
#include "UIKit/ResourceUtils.h"

using namespace d14engine::renderer;

namespace d14engine::uikit
{
    ScrollView::ScrollView(
        ShrdPtrRefer<Panel> content,
        const D2D1_RECT_F& rect)
        :
        Panel(rect, resource_utils::solidColorBrush()),
        ResizablePanel(rect, resource_utils::solidColorBrush()),
        contentMask(size()),
        m_content(content)
    {
        m_childHitTestRect = m_rect;

        setResizable(false);
    }

    void ScrollView::onInitializeFinish()
    {
        ResizablePanel::onInitializeFinish();

        registerUIEvents(m_content);

        if (m_content) m_content->setPosition(0.0f, 0.0f);
    }

    void ScrollView::onStartThumbScrolling(const D2D1_POINT_2F& offset)
    {
        onStartThumbScrollingHelper(offset);

        if (f_onStartThumbScrolling) f_onStartThumbScrolling(this, offset);
    }

    void ScrollView::onEndThumbScrolling(const D2D1_POINT_2F& offset)
    {
        onEndThumbScrollingHelper(offset);

        if (f_onEndThumbScrolling) f_onEndThumbScrolling(this, offset);
    }

    void ScrollView::onViewportOffsetChange(const D2D1_POINT_2F& offset)
    {
        onViewportOffsetChangeHelper(offset);

        if (f_onViewportOffsetChange) f_onViewportOffsetChange(this, offset);
    }

    void ScrollView::onStartThumbScrollingHelper(const D2D1_POINT_2F& offset)
    {
        THROW_IF_NULL(Application::g_app);

        m_enableChildMouseMoveEvent = false;

        auto focus = Application::FocusType::Mouse;
        Application::g_app->focusUIObject(focus, shared_from_this());
    }

    void ScrollView::onEndThumbScrollingHelper(const D2D1_POINT_2F& offset)
    {
        THROW_IF_NULL(Application::g_app);

        m_enableChildMouseMoveEvent = true;

        auto focus = Application::FocusType::Mouse;
        Application::g_app->focusUIObject(focus, nullptr);
    }

    void ScrollView::onViewportOffsetChangeHelper(const D2D1_POINT_2F& offset)
    {
        if (m_content) m_content->setPosition(-offset.x, -offset.y);
    }

    D2D1_SIZE_F ScrollView::getViewSize() const
    {
        if (customSizeGetter.view) return customSizeGetter.view(this);
        else return size();
    }

    D2D1_SIZE_F ScrollView::getContentSize() const
    {
        if (customSizeGetter.content) return customSizeGetter.content(this);
        else if (m_content) return m_content->size();
        else return { 0.0f, 0.0f };
    }

    void ScrollView::setContent(ShrdPtrRefer<Panel> content)
    {
        if (!cpp_lang_utils::isMostDerivedEqual(content, m_content))
        {
            unregisterUIEvents(m_content);

            m_content = content;
            registerUIEvents(m_content);

            if (m_content) m_content->setPosition(0.0f, 0.0f);

            m_viewportOffset = { 0.0f, 0.0f };
        }
    }

    D2D1_POINT_2F ScrollView::validateViewportOffset(const D2D1_POINT_2F& in)
    {
        D2D1_POINT_2F out = { 0.0f, 0.0f };

        auto viewSize = getViewSize();
        auto contentSize = getContentSize();

        if (contentSize.width > viewSize.width)
        {
            out.x = std::clamp(in.x, 0.0f, contentSize.width - viewSize.width);
        }
        else out.x = 0.0f;

        if (contentSize.height > viewSize.height)
        {
            out.y = std::clamp(in.y, 0.0f, contentSize.height - viewSize.height);
        }
        else out.y = 0.0f;

        return out; // valid viewport offset
    }

    const D2D1_POINT_2F& ScrollView::viewportOffset() const
    {
        return m_viewportOffset;
    }

    void ScrollView::setViewportOffset(const D2D1_POINT_2F& absolute)
    {
        auto validOffset = validateViewportOffset(absolute);

        if (validOffset.x != m_viewportOffset.x ||
            validOffset.y != m_viewportOffset.y)
        {
            m_viewportOffset = validOffset;
            onViewportOffsetChange(validOffset);
        }
    }

    D2D1_POINT_2F ScrollView::viewportOffsetPercentage() const
    {
        auto viewSize = getViewSize();
        auto contentSize = getContentSize();
        return
        {
            m_viewportOffset.x / (contentSize.width - viewSize.width),
            m_viewportOffset.y / (contentSize.height - viewSize.height)
        };
    }

    void ScrollView::setViewportOffsetPercentage(const D2D1_POINT_2F& relative)
    {
        auto viewSize = getViewSize();
        auto contentSize = getContentSize();
        setViewportOffset
        ({
            relative.x * (contentSize.width - viewSize.width),
            relative.y * (contentSize.height - viewSize.height)
        });
    }

    bool ScrollView::isControllingHorzBar() const
    {
        return m_isHorzBarHover || m_isHorzBarDown;
    }

    bool ScrollView::isControllingVertBar() const
    {
        return m_isVertBarHover || m_isVertBarDown;
    }

    bool ScrollView::isControllingScrollBars() const
    {
        return isControllingHorzBar() || isControllingVertBar();
    }

    ScrollView::ScrollBarState ScrollView::getHorzBarState(bool isHover, bool isDown) const
    {
        if (m_isHorzBarDown) return ScrollBarState::Down;
        else if (m_isHorzBarHover) return ScrollBarState::Hover;
        else return ScrollBarState::Idle;
    }

    ScrollView::ScrollBarState ScrollView::getVertBarState(bool isHover, bool isDown) const
    {
        if (m_isVertBarDown) return ScrollBarState::Down;
        else if (m_isVertBarHover) return ScrollBarState::Hover;
        else return ScrollBarState::Idle;
    }

    D2D1_RECT_F ScrollView::horzBarSelfcoordRect(ScrollBarState state) const
    {
        auto& setting = appearance();
        auto& geoSetting = setting.scrollBar[(size_t)state].geometry;

        auto viewSize = getViewSize();
        auto contentSize = getContentSize();

        D2D1_RECT_F rect =
        {
            0.0f, height() - (geoSetting.offset + geoSetting.width),
            0.0f, height() - geoSetting.offset
        };
        if (viewSize.width > 0.0f && contentSize.width > viewSize.width)
        {
            float horzStart = m_viewportOffset.x / contentSize.width;
            float horzEnd = horzStart + viewSize.width / contentSize.width;

            rect.left = std::round(horzStart * viewSize.width);
            rect.right = std::round(horzEnd * viewSize.width);
        }
        return rect;
    }

    D2D1_RECT_F ScrollView::vertBarSelfcoordRect(ScrollBarState state) const
    {
        auto& setting = appearance();
        auto& geoSetting = appearance().scrollBar[(size_t)state].geometry;

        auto viewSize = getViewSize();
        auto contentSize = getContentSize();

        D2D1_RECT_F rect =
        {
            width() - (geoSetting.offset + geoSetting.width), 0.0f,
            width() - geoSetting.offset, 0.0f
        };
        if (viewSize.height > 0.0f && contentSize.height > viewSize.height)
        {
            float vertStart = m_viewportOffset.y / contentSize.height;
            float vertEnd = vertStart + viewSize.height / contentSize.height;

            rect.top = std::round(vertStart * viewSize.height);
            rect.bottom = std::round(vertEnd * viewSize.height);
        }
        return rect;
    }

    void ScrollView::onRendererUpdateObject2DHelper(Renderer* rndr)
    {
        ResizablePanel::onRendererUpdateObject2DHelper(rndr);

        if (m_content && m_content->isD2d1ObjectVisible())
        {
            m_content->onRendererUpdateObject2D(rndr);
        }
    }

    void ScrollView::onRendererDrawD2d1LayerHelper(Renderer* rndr)
    {
        if (m_content && m_content->isD2d1ObjectVisible())
        {
            m_content->onRendererDrawD2d1Layer(rndr);

            //--------------------------------------------------------------
            // 1. Grayscale text anti-aliasing:
            // The rendering result is independent of the target background,
            // so opacity can be set as needed (any value from 0 ~ 1 is OK).
            //--------------------------------------------------------------
            // 2. ClearType text anti-aliasing:
            // The rendering result depends on the target background color,
            // so you must set an opaque background (better a value >= 0.5).
            //--------------------------------------------------------------
            auto& bkgn = appearance().background;

            contentMask.color = bkgn.color;
            contentMask.color.a = bkgn.opacity;

            auto maskTrans = D2D1::Matrix3x2F::Translation
            (
                -m_absoluteRect.left, -m_absoluteRect.top
            );
            contentMask.beginDraw(rndr->d2d1DeviceContext(), maskTrans);
            {
                m_content->onRendererDrawD2d1Object(rndr);
            }
            contentMask.endDraw(rndr->d2d1DeviceContext());
        }
    }

    void ScrollView::onRendererDrawD2d1ObjectHelper(Renderer* rndr)
    {
        ////////////////
        // Background //
        ////////////////

        auto& bkgn = appearance().background;

        resource_utils::solidColorBrush()->SetColor(bkgn.color);
        resource_utils::solidColorBrush()->SetOpacity(bkgn.opacity);

        ResizablePanel::drawBackground(rndr);

        /////////////
        // Content //
        /////////////

        if (m_content && m_content->isD2d1ObjectVisible())
        {
            rndr->d2d1DeviceContext()->DrawBitmap
            (
            /* bitmap               */ contentMask.data.Get(),
            /* destinationRectangle */ m_absoluteRect,
            /* opacity              */ contentMask.opacity,
            /* interpolationMode    */ contentMask.getInterpolationMode()
            );
        }
        /////////////
        // Outline //
        /////////////

        auto& stroke = appearance().stroke;

        resource_utils::solidColorBrush()->SetColor(stroke.color);
        resource_utils::solidColorBrush()->SetOpacity(stroke.opacity);

        auto frame = math_utils::inner(m_absoluteRect, stroke.width);
        D2D1_ROUNDED_RECT outlineRect = { frame, roundRadiusX, roundRadiusY };

        rndr->d2d1DeviceContext()->DrawRoundedRectangle
        (
        /* roundedRect */ outlineRect,
        /* brush       */ resource_utils::solidColorBrush(),
        /* strokeWidth */ stroke.width
        );
    }

    void ScrollView::drawD2d1ObjectPosterior(renderer::Renderer* rndr)
    {
        //////////////
        // Horz Bar //
        //////////////

        if (isHorzBarEnabled)
        {
            auto state = getHorzBarState(m_isHorzBarHover, m_isHorzBarDown);
            auto& setting = appearance().scrollBar[(size_t)state];

            resource_utils::solidColorBrush()->SetColor(setting.background.color);
            resource_utils::solidColorBrush()->SetOpacity(setting.background.opacity);

            D2D1_ROUNDED_RECT roundedRect =
            {
                selfCoordToAbsolute(horzBarSelfcoordRect(state)),
                setting.geometry.roundRadius, setting.geometry.roundRadius
            };
            rndr->d2d1DeviceContext()->FillRoundedRectangle
            (
            /* roundedRect */ roundedRect,
            /* brush       */ resource_utils::solidColorBrush()
            );
        }
        //////////////
        // Vert Bar //
        //////////////

        if (isVertBarEnabled)
        {
            auto state = getVertBarState(m_isVertBarHover, m_isVertBarDown);
            auto& setting = appearance().scrollBar[(size_t)state];

            resource_utils::solidColorBrush()->SetColor(setting.background.color);
            resource_utils::solidColorBrush()->SetOpacity(setting.background.opacity);

            D2D1_ROUNDED_RECT roundedRect =
            {
                selfCoordToAbsolute(vertBarSelfcoordRect(state)),
                setting.geometry.roundRadius, setting.geometry.roundRadius
            };
            rndr->d2d1DeviceContext()->FillRoundedRectangle
            (
            /* roundedRect */ roundedRect,
            /* brush       */ resource_utils::solidColorBrush()
            );
        }
        ResizablePanel::drawD2d1ObjectPosterior(rndr);
    }

    bool ScrollView::releaseUIObjectHelper(ShrdPtrRefer<Panel> uiobj)
    {
        if (cpp_lang_utils::isMostDerivedEqual(m_content, uiobj)) m_content.reset();

        return ResizablePanel::releaseUIObjectHelper(uiobj);
    }

    void ScrollView::onSizeHelper(SizeEvent& e)
    {
        ResizablePanel::onSizeHelper(e);

        contentMask.loadBitmap(e.size);

        // The viewport offset may be invalid after resizing.
        setViewportOffset(m_viewportOffset);
    }

    void ScrollView::onChangeThemeStyleHelper(const ThemeStyle& style)
    {
        ResizablePanel::onChangeThemeStyleHelper(style);

        appearance().changeTheme(style.name);
    }

    void ScrollView::onMouseMoveHelper(MouseMoveEvent& e)
    {
        auto p = absoluteToSelfCoord(e.cursorPoint);

        //////////////////////////////
        // Update scroll bar state. //
        //////////////////////////////

        // This has been initialized in the ctor and is guaranteed to be valid.
        auto& hitTestRect = m_childHitTestRect.value();

        if (isHorzBarEnabled)
        {
            auto state = m_isHorzBarHover ?
                ScrollBarState::Hover : ScrollBarState::Idle;

            auto rect = horzBarSelfcoordRect(state);
            rect = math_utils::overrideBottom(rect, FLT_MAX);

            // Since the judgment of m_childHitTestRect uses isOverlapped,
            // using isOverlapped here may cause conflicts when the cursor
            // moves to the boundary between the scroll bar and the hit-test rect.
            // However, the probability of this situation occurring is very low,
            // so using isOverlapped here for consistency is also acceptable.

            //m_isHorzBarHover = math_utils::isInside(p, rect);
            m_isHorzBarHover = math_utils::isOverlapped(p, rect);

            // Note that this should be top rather than bottom.
            hitTestRect.bottom = rect.top;
        }
        else hitTestRect.bottom = m_rect.bottom;

        if (isVertBarEnabled)
        {
            auto state = m_isVertBarHover ?
                ScrollBarState::Hover : ScrollBarState::Idle;

            auto rect = vertBarSelfcoordRect(state);
            rect = math_utils::overrideRight(rect, FLT_MAX);

            // Refer to the comments above about the update of m_isHorzBarHover.
            //m_isVertBarHover = math_utils::isInside(p, rect);
            m_isVertBarHover = math_utils::isOverlapped(p, rect);

            // Note that this should be left rather than right.
            hitTestRect.right = rect.left;
        }
        else hitTestRect.right = m_rect.right;

        // Call helper here to ensure the updated m_childHitTestRect is used.
        ResizablePanel::onMouseMoveHelper(e);

        //////////////////////////////
        // Perform viewport motion. //
        //////////////////////////////

        auto viewSize = getViewSize();

        if (viewSize.width > 0.0f && viewSize.height > 0.0f)
        {
            auto contentSize = getContentSize();

            float horzRatio = contentSize.width / viewSize.width;
            float vertRatio = contentSize.height / viewSize.height;

            if (m_isHorzBarDown)
            {
                auto deltaX = std::round
                (
                    (p.x - m_horzBarHoldOffset) * horzRatio
                );
                setViewportOffset(math_utils::offset
                (
                    m_originalViewportOffset, { deltaX, 0.0f }
                ));
            }
            if (m_isVertBarDown)
            {
                auto deltaY = std::round
                (
                    (p.y - m_vertBarHoldOffset) * vertRatio
                );
                setViewportOffset(math_utils::offset
                (
                    m_originalViewportOffset, { 0.0f, deltaY }
                ));
            }
        }
    }

    void ScrollView::onMouseLeaveHelper(MouseMoveEvent& e)
    {
        ResizablePanel::onMouseLeaveHelper(e);

        if (!holdMouseFocus())
        {
            m_isHorzBarHover = m_isHorzBarDown =
            m_isVertBarHover = m_isVertBarDown = false;
        }
    }

    void ScrollView::onMouseButtonHelper(MouseButtonEvent& e)
    {
        ResizablePanel::onMouseButtonHelper(e);

        auto p = absoluteToSelfCoord(e.cursorPoint);

        if (e.state.leftDown() || e.state.leftDblclk())
        {
            if (isHorzBarEnabled)
            {
                m_isHorzBarDown = m_isHorzBarHover;
            }
            if (isVertBarEnabled)
            {
                m_isVertBarDown = m_isVertBarHover;
            }
            if (isControllingScrollBars())
            {
                onStartThumbScrolling(m_viewportOffset);
            }
            m_horzBarHoldOffset = p.x;
            m_vertBarHoldOffset = p.y;

            m_originalViewportOffset = m_viewportOffset;
        }
        else if (e.state.leftUp())
        {
            if (isControllingScrollBars())
            {
                onEndThumbScrolling(m_viewportOffset);
            }
            m_isHorzBarDown = m_isVertBarDown = false;
            m_horzBarHoldOffset = m_vertBarHoldOffset = 0.0f;
            m_originalViewportOffset = { 0.0f, 0.0f };
        }
    }

    void ScrollView::onMouseWheelHelper(MouseWheelEvent& e)
    {
        ResizablePanel::onMouseWheelHelper(e);

        THROW_IF_NULL(Application::g_app);

        D2D1_POINT_2F nextOffset = m_viewportOffset;

        if (e.keyState.SHIFT)
        {
            nextOffset.x -= e.deltaCount * deltaDipsPerScroll.horz;
        }
        else nextOffset.y -= e.deltaCount * deltaDipsPerScroll.vert;

        setViewportOffset(nextOffset);

        Application::g_app->sendNextImmediateMouseMoveEvent = true;
    }
}
