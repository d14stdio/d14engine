#pragma once

#include "Common/Precompile.h"

#include "UIKit/Appearances/ElevatedButton.h"
#include "UIKit/FilledButton.h"
#include "UIKit/ShadowMask.h"

namespace d14engine::uikit
{
    struct ElevatedButton : appearance::ElevatedButton, FilledButton
    {
        ElevatedButton(
            SharedPtrParam<IconLabel> content,
            float roundRadius = 0.0f,
            const D2D1_RECT_F& rect = {});

        ElevatedButton(
            WstrParam text = L"Button",
            float roundRadius = 0.0f,
            const D2D1_RECT_F& rect = {});

        _D14_SET_APPEARANCE_PROPERTY(ElevatedButton)

        //////////////////////
        // Cached Resources //
        //////////////////////

        ShadowMask shadow = {};

        /////////////////////////
        // Interface Overrides //
        /////////////////////////

    protected:
        //------------------------------------------------------------------
        // IDrawObject2D
        //------------------------------------------------------------------

        void onRendererDrawD2d1LayerHelper(Renderer* rndr) override;

        void onRendererDrawD2d1ObjectHelper(Renderer* rndr) override;

        //------------------------------------------------------------------
        // Panel
        //------------------------------------------------------------------

        void onSizeHelper(SizeEvent& e) override;

        void onThemeStyleChangedHelper(const ThemeStyle& style) override;
    };
}
