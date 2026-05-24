#pragma once

#include "Common/Precompile.h"

#include "UIKit/Panel.h"

namespace d14engine::uikit
{
    struct DraggablePanel : virtual Panel
    {
        DraggablePanel(
            const D2D1_RECT_F& rect = {},
            ComPtrParam<ID2D1Brush> brush = nullptr,
            ComPtrParam<ID2D1Bitmap1> bitmap = nullptr);

        ////////////////////////
        // Callback Functions //
        ////////////////////////

        //------------------------------------------------------------------
        // Public Interfaces
        //------------------------------------------------------------------
    public:
        void onDragStart();

        Function<void(DraggablePanel*)> f_onDragStart = {};

        void onDragEnd();

        Function<void(DraggablePanel*)> f_onDragEnd = {};

        bool isDragAreaHit(const Event::Point& p);

        Function<bool(DraggablePanel*, const Event::Point&)> f_isDragAreaHit = {};

        //------------------------------------------------------------------
        // Protected Helpers
        //------------------------------------------------------------------
    protected:
        virtual void onDragStartHelper();
        virtual void onDragEndHelper();

        virtual bool isDragAreaHitHelper(const Event::Point& p);

        ///////////////////////
        // Interaction Logic //
        ///////////////////////

        //------------------------------------------------------------------
        // Draggable
        //------------------------------------------------------------------
    public:
        bool draggable = true;

        //------------------------------------------------------------------
        // Drag Target
        //------------------------------------------------------------------
    public:
        enum class DragTarget
        {
            SelfObject, RootWindow
        };
        constexpr static auto SelfObject = DragTarget::SelfObject;
        constexpr static auto RootWindow = DragTarget::RootWindow;

        DragTarget dragTarget = SelfObject;

        //------------------------------------------------------------------
        // Drag State
        //------------------------------------------------------------------
    protected:
        bool m_isDragging = false;

    public:
        bool isDragging() const;

        //------------------------------------------------------------------
        // Drag Point
        //------------------------------------------------------------------
    protected:
        using SelfPoint = D2D1_POINT_2F;
        using RootPoint = POINT;

        using DragPoint = Variant<std::monostate, SelfPoint, RootPoint>;

        DragPoint m_dragPoint = {};

    public:
        const DragPoint& dragPoint() const;

        /////////////////////////
        // Interface Overrides //
        /////////////////////////

    protected:
        //------------------------------------------------------------------
        // Panel
        //------------------------------------------------------------------

        void onMouseMoveHelper(MouseMoveEvent& e) override;
        void onMouseMoveWrapper(MouseMoveEvent& e);

        void onMouseButtonHelper(MouseButtonEvent& e) override;
        void onMouseButtonWrapper(MouseButtonEvent& e);
    };
}
