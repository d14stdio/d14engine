#include "Common/Precompile.h"

#include "UIKit/DraggablePanel.h"

#include "Common/RuntimeError.h"

#include "UIKit/Application.h"
#include "UIKit/Cursor.h"

namespace d14engine::uikit
{
    DraggablePanel::DraggablePanel(
        const D2D1_RECT_F& rect,
        ComPtrParam<ID2D1Brush> brush,
        ComPtrParam<ID2D1Bitmap1> bitmap)
        :
        Panel(rect, brush, bitmap)
    {
        // Here left blank intentionally.
    }

    void DraggablePanel::onDragStart()
    {
        onDragStartHelper();

        if (f_onDragStart) f_onDragStart(this);
    }

    void DraggablePanel::onDragEnd()
    {
        onDragEndHelper();

        if (f_onDragEnd) f_onDragEnd(this);
    }

    bool DraggablePanel::isDragAreaHit(const Event::Point& p)
    {
        if (!draggable) return false;

        if (f_isDragAreaHit)
        {
            return f_isDragAreaHit(this, p);
        }
        else return isDragAreaHitHelper(p);
    }

    void DraggablePanel::onDragStartHelper()
    {
        THROW_IF_NULL(Application::g_app);

        auto& app = Application::g_app;

        m_enableChildMouseMoveEvent = false;

        app->focusUIObject
        (
            Application::FocusType::Mouse, shared_from_this()
        );
        if (dragTarget == RootWindow)
        {
            app->m_isDraggingWin32Window = true;
        }
        app->cursor()->setIcon(Cursor::Move);
    }

    void DraggablePanel::onDragEndHelper()
    {
        THROW_IF_NULL(Application::g_app);

        auto& app = Application::g_app;

        m_enableChildMouseMoveEvent = true;

        app->focusUIObject
        (
            Application::FocusType::Mouse, nullptr
        );
        if (dragTarget == RootWindow)
        {
            app->m_isDraggingWin32Window = false;
        }
        app->cursor()->setIcon(Cursor::Arrow);
    }

    bool DraggablePanel::isDragAreaHitHelper(const Event::Point& p)
    {
        return isHit(p);
    }

    bool DraggablePanel::isDragging() const
    {
        return m_isDragging;
    }

    const DraggablePanel::DragPoint& DraggablePanel::dragPoint() const
    {
        return m_dragPoint;
    }

    void DraggablePanel::onMouseMoveHelper(MouseMoveEvent& e)
    {
        Panel::onMouseMoveHelper(e);

        onMouseMoveWrapper(e);
    }

    void DraggablePanel::onMouseMoveWrapper(MouseMoveEvent& e)
    {
        THROW_IF_NULL(Application::g_app);

        auto& p = e.cursorPoint;

        if (m_isDragging)
        {
            switch (dragTarget)
            {
            case SelfObject:
            {
                if (std::holds_alternative<SelfPoint>(m_dragPoint))
                {
                    auto& point = std::get<SelfPoint>(m_dragPoint);

                    auto relative = absoluteToRelative(p);
                    setPosition(relative.x - point.x, relative.y - point.y);
                }
                break;
            }
            case RootWindow:
            {
                if (std::holds_alternative<RootPoint>(m_dragPoint))
                {
                    auto& point = std::get<RootPoint>(m_dragPoint);

                    POINT cursorPoint = {};
                    GetCursorPos(&cursorPoint);

                    int X = cursorPoint.x - point.x;
                    int Y = cursorPoint.y - point.y;

                    auto hwnd = Application::g_app->win32Window();
                    SetWindowPos(hwnd, HWND_TOP, X, Y, 0, 0, SWP_NOSIZE);
                }
                break;
            }
            default: break;
            }
            Application::g_app->cursor()->setIcon(Cursor::Move);
        }
    }

    void DraggablePanel::onMouseButtonHelper(MouseButtonEvent& e)
    {
        Panel::onMouseButtonHelper(e);

        onMouseButtonWrapper(e);
    }

    void DraggablePanel::onMouseButtonWrapper(MouseButtonEvent& e)
    {
        THROW_IF_NULL(Application::g_app);

        auto& p = e.cursorPoint;

        if (e.state.leftDown() || e.state.leftDblclk())
        {
            if (m_isDragging = isDragAreaHit(p))
            {
                switch (dragTarget)
                {
                case SelfObject:
                {
                    m_dragPoint = absoluteToSelfCoord(p);
                    break;
                }
                case RootWindow:
                {
                    POINT cursorPoint = {};
                    GetCursorPos(&cursorPoint);
                    ScreenToClient(Application::g_app->win32Window(), &cursorPoint);

                    m_dragPoint = cursorPoint;
                    break;
                }
                default: m_dragPoint = {};
                }
                onDragStart();
            }
        }
        else if (e.state.leftUp())
        {
            if (m_isDragging)
            {
                m_isDragging = false;

                onDragEnd();
            }
        }
    }
}
