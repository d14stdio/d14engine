#pragma once

#include "Common/Precompile.h"

#include "IconLabel.h"

namespace d14engine::uikit
{
    struct IconLabel2 : IconLabel
    {
        IconLabel2(
            WstrRefer labelText = {},
            WstrRefer labelText2 = {},
            BmpObjParam iconBitmap = {},
            const D2D1_RECT_F& rect = {});

        void initialize() override;

        //////////////////////////
        // Graphical Components //
        //////////////////////////

        //------------------------------------------------------------------
        // Child Objects
        //------------------------------------------------------------------

    protected:
        SharedPtr<Label> m_label2 = {};

    public:
        const SharedPtr<Label>& label2() const;
        void setLabel2(ShrdPtrRefer<Label> label);

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

        bool releaseUIObjectHelper(ShrdPtrRefer<Panel> uiobj) override;

        //////////////////////
        // Layout Templates //
        //////////////////////

    public:
        static SharedPtr<IconLabel2> menuItemLayout(
            WstrRefer labelText = {},
            WstrRefer hotkeyText = {},
            BmpObjParam iconBitmap = {},
            float textHeadPadding = 30.0f,
            float hotkeyTailPadding = 30.0f,
            const D2D1_RECT_F& rect = {});
    };
}
