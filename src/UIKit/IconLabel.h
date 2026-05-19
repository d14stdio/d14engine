#pragma once

#include "Common/Precompile.h"

#include "UIKit/BitmapObject.h"
#include "UIKit/Panel.h"

namespace d14engine::uikit
{
    struct Label;

    struct IconLabel : Panel
    {
        IconLabel(
            WstrParam labelText = {},
            BmpObjParam iconBitmap = {},
            const D2D1_RECT_F& rect = {});

        void initialize() override;

        //////////////////////////
        // Graphical Components //
        //////////////////////////

        //------------------------------------------------------------------
        // Child Objects
        //------------------------------------------------------------------
    public:
        struct Icon
        {
            D2D1_RECT_F rect = {};
            BitmapObject bitmap = {};

            Optional<D2D1_SIZE_F> size = {};
        }
        icon = {};

    protected:
        SharedPtr<Label> m_label = {};

    public:
        const SharedPtr<Label>& label() const;
        void setLabel(SharedPtrParam<Label> label);

        ///////////////////////
        // Interaction Logic //
        ///////////////////////

        //------------------------------------------------------------------
        // Update Layout
        //------------------------------------------------------------------
    public:
        void updateLayout();

        Function<void(IconLabel*)> f_updateLayout = {};

    protected:
        virtual void updateLayoutHelper();

        /////////////////////////
        // Interface Overrides //
        /////////////////////////

    public:
        //------------------------------------------------------------------
        // Panel
        //------------------------------------------------------------------

        void setEnabled(bool value) override;

    protected:
        //------------------------------------------------------------------
        // IDrawObject2D
        //------------------------------------------------------------------

        void onRendererDrawD2d1ObjectHelper(Renderer* rndr) override;

        //------------------------------------------------------------------
        // Panel
        //------------------------------------------------------------------

        bool releaseUIObjectHelper(SharedPtrParam<Panel> uiobj) override;

        void onSizeHelper(SizeEvent& e) override;

        //////////////////////
        // Layout Templates //
        //////////////////////

    public:
        static SharedPtr<IconLabel> uniformLayout(
            WstrParam labelText = {},
            BmpObjParam iconBitmap = {},
            const D2D1_RECT_F& rect = {});

        static SharedPtr<IconLabel> compactLayout(
            WstrParam labelText = {},
            BmpObjParam iconBitmap = {},
            float iconHeadPadding = 0.0f,
            float iconTailPadding = 0.0f,
            const D2D1_RECT_F& rect = {});

        static SharedPtr<IconLabel> iconExpandedLayout(
            WstrParam labelText = {},
            BmpObjParam iconBitmap = {},
            const D2D1_RECT_F& rect = {});

        static SharedPtr<IconLabel> labelExpandedLayout(
            WstrParam labelText = {},
            BmpObjParam iconBitmap = {},
            const D2D1_RECT_F& rect = {});

        static SharedPtr<IconLabel> comboBoxLayout(
            WstrParam labelText = {},
            BmpObjParam iconBitmap = {},
            const D2D1_RECT_F& rect = {});
    };
}
