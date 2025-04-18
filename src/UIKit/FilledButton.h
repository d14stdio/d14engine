﻿#pragma once

#include "Common/Precompile.h"

#include "UIKit/Appearances/FilledButton.h"
#include "UIKit/FlatButton.h"

namespace d14engine::uikit
{
    struct FilledButton : appearance::FilledButton, FlatButton
    {
        FilledButton(
            ShrdPtrRefer<IconLabel> content,
            float roundRadius = 0.0f,
            const D2D1_RECT_F& rect = {});

        FilledButton(
            WstrRefer text = L"Button",
            float roundRadius = 0.0f,
            const D2D1_RECT_F& rect = {});

        _D14_SET_APPEARANCE_PROPERTY(FilledButton)

    protected:
        // Panel
        void onChangeThemeStyleHelper(const ThemeStyle& style) override;
    };
}
