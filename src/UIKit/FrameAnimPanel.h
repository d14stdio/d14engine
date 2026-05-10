#pragma once

#include "Common/Precompile.h"

#include "UIKit/AnimationUtils/BitmapSequence.h"
#include "UIKit/Panel.h"

namespace d14engine::uikit
{
    struct FrameAnimPanel : Panel
    {
        using Panel::Panel;

        //////////////////////////
        // Graphical Components //
        //////////////////////////

        //------------------------------------------------------------------
        // Bitmap Data
        //------------------------------------------------------------------
    public:
        animation_utils::BitmapSequence bitmapData = {};

        /////////////////////////
        // Interface Overrides //
        /////////////////////////

    protected:
        //------------------------------------------------------------------
        // IDrawObject2D
        //------------------------------------------------------------------

        void onRendererUpdateObject2DHelper(Renderer* rndr) override;

        void onRendererDrawD2d1ObjectHelper(Renderer* rndr) override;
    };
}
