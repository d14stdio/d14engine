﻿#pragma once

#include "Common/Precompile.h"

#include "UIKit/Appearances/TextInput.h"
#include "UIKit/RawTextInput.h"

namespace d14engine::uikit
{
    struct TextInput : appearance::TextInput, RawTextInput
    {
        using RawTextInput::RawTextInput;

        _D14_SET_APPEARANCE_PROPERTY(TextInput)

        void setEnabled(bool value) override;

    protected:
        State m_state = State::Idle;

        float m_dynamicBottomLineLength = 0.0f;

    protected:
        // IDrawObject2D
        void onRendererUpdateObject2DHelper(renderer::Renderer* rndr) override;

        void onRendererDrawD2d1LayerHelper(renderer::Renderer* rndr) override;

        void onRendererDrawD2d1ObjectHelper(renderer::Renderer* rndr) override;

        // Panel
        void onGetKeyboardFocusHelper() override;

        void onLoseKeyboardFocusHelper() override;

        void onMouseEnterHelper(MouseMoveEvent& e) override;

        void onMouseLeaveHelper(MouseMoveEvent& e) override;

        void onMouseButtonHelper(MouseButtonEvent& e) override;

        void onChangeThemeStyleHelper(const ThemeStyle& style) override;
    };
}
