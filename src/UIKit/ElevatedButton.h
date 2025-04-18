﻿#pragma once

#include "Common/Precompile.h"

#include "UIKit/Appearances/ElevatedButton.h"
#include "UIKit/FilledButton.h"
#include "UIKit/ShadowMask.h"

namespace d14engine::uikit
{
    struct ElevatedButton : appearance::ElevatedButton, FilledButton
    {
        ElevatedButton(
            ShrdPtrRefer<IconLabel> content,
            float roundRadius = 0.0f,
            const D2D1_RECT_F& rect = {});

        ElevatedButton(
            WstrRefer text = L"Button",
            float roundRadius = 0.0f,
            const D2D1_RECT_F& rect = {});

        ShadowMask shadow = {};

        _D14_SET_APPEARANCE_PROPERTY(ElevatedButton)

    protected:
        // IDrawObject2D
        void onRendererDrawD2d1LayerHelper(renderer::Renderer* rndr) override;

        void onRendererDrawD2d1ObjectHelper(renderer::Renderer* rndr) override;

        // Panel
        void onSizeHelper(SizeEvent& e) override;

        void onChangeThemeStyleHelper(const ThemeStyle& style) override;
    };
}
