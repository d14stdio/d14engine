﻿#pragma once

#include "Common/Precompile.h"

#include "UIKit/Appearances/FlatButton.h"

#include "UIKit/Button.h"

namespace d14engine::uikit
{
    struct FlatButton : appearance::FlatButton, Button
    {
        FlatButton(
            ShrdPtrRefer<IconLabel> content,
            float roundRadius = 0.0f,
            const D2D1_RECT_F& rect = {});

        FlatButton(
            WstrRefer text = L"Button",
            float roundRadius = 0.0f,
            const D2D1_RECT_F& rect = {});

        _D14_SET_APPEARANCE_PROPERTY(FlatButton)

    protected:
        // IDrawObject2D
        void onRendererDrawD2d1ObjectHelper(renderer::Renderer* rndr) override;

        // Panel
        void onChangeThemeStyleHelper(const ThemeStyle& style) override;
    };
}
