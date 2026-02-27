#pragma once

#include "Common/Precompile.h"

#include "Common/MathUtils/2D.h"

#include "UIKit/Appearances/LabelArea.h"
#include "UIKit/Label.h"

namespace d14engine::uikit
{
    struct LabelArea : appearance::LabelArea, Label
    {
        LabelArea(WstrRefer text = {}, const D2D_RECT_F& rect = {});

        _D14_SET_APPEARANCE_PROPERTY(LabelArea)

    protected:
        // Controls the blinking of the caret:
        // (True) Show the caret. (False) Hide the caret.
        bool m_caretBlinkingFlag = false;

        std::pair<D2D1_POINT_2F, D2D1_POINT_2F> m_caretGeometry = {};
        size_t m_caretPosition = 0;

        virtual size_t hitTestCaretPosition(const D2D1_POINT_2F& point);

        float m_caretBlinkingElapsedSecs = 0.0f;

    public:
        bool keepCaretPosition = false;

        D2D1_RECT_F caretConstrainedRect = math_utils::infiniteRectF();

        size_t caretPosition() const;
        virtual void setCaretPosition(size_t position);

    protected:
        size_t m_selectedRangeOrigin = 0;

        CharacterRange m_selectedRange = { 0, 0 };

        TextRangeHitTestResult m_selectedRangeData = {};

    public:
        bool keepSelectedRange = false;

        const CharacterRange& selectedRange() const;
        void setSelectedRange(const CharacterRange& range);

    public:
        virtual void setSelectedText(WstrRefer text);

    protected:
        // Returns whether m_text is changed after called.
        bool setSelectedTextHelper(WstrRefer text);

    public:
        virtual void performCommandSelectAll();
        virtual void performCommandCopySelection();

    protected:
        // IDrawObject2D
        void onRendererUpdateObject2DHelper(Renderer* rndr) override;

        void onRendererDrawD2d1ObjectHelper(Renderer* rndr) override;

        void drawCaret(Renderer* rndr);
        void drawSelection(Renderer* rndr);

        // Panel
        void onGetKeyboardFocusHelper() override;

        void onLoseKeyboardFocusHelper() override;

        void onSizeHelper(SizeEvent& e) override;

        void onMouseMoveHelper(MouseMoveEvent& e) override;

        void onMouseButtonHelper(MouseButtonEvent& e) override;

        void onKeyboardHelper(KeyboardEvent& e) override;

        void onChangeThemeStyleHelper(const ThemeStyle& style) override;

        // Label
        void onTextLayoutChangeHelper() override;
    };
}
