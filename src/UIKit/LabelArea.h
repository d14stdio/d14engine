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

        //////////////////////////
        // Graphical Components //
        //////////////////////////

        //------------------------------------------------------------------
        // Caret Position
        //------------------------------------------------------------------
    protected:
        size_t m_caretPosition = 0;
        std::pair<D2D1_POINT_2F, D2D1_POINT_2F> m_caretGeometry = {};

        virtual size_t hitTestCaretPosition(const D2D1_POINT_2F& point);

    public:
        bool keepCaretPosition = false;

        D2D1_RECT_F caretConstrainedRect = math_utils::infiniteRectF();

        size_t caretPosition() const;
        virtual void setCaretPosition(size_t position);

        //------------------------------------------------------------------
        // Caret Blinking
        //------------------------------------------------------------------
    protected:
        // This is an internal variable used in the implementation
        // of caret blinking, and it is not intended to be set directly.
        bool m_caretBlinkingFlag = false;

        float m_caretBlinkingElapsedSecs = 0.0f;

        //------------------------------------------------------------------
        // Selected Range
        //------------------------------------------------------------------
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

        ///////////////////////
        // Interaction Logic //
        ///////////////////////

        //------------------------------------------------------------------
        // Shortcut Commands
        //------------------------------------------------------------------

    public:
        virtual void performCommandSelectAll();
        virtual void performCommandCopySelection();

        /////////////////////////
        // Interface Overrides //
        /////////////////////////

    protected:
        //------------------------------------------------------------------
        // IDrawObject2D
        //------------------------------------------------------------------

        void onRendererUpdateObject2DHelper(Renderer* rndr) override;

        void onRendererDrawD2d1ObjectHelper(Renderer* rndr) override;

        void drawCaret(Renderer* rndr);
        void drawSelection(Renderer* rndr);

        //------------------------------------------------------------------
        // Panel
        //------------------------------------------------------------------

        void onGetKeyboardFocusHelper() override;

        void onLoseKeyboardFocusHelper() override;

        void onSizeHelper(SizeEvent& e) override;

        void onMouseMoveHelper(MouseMoveEvent& e) override;

        void onMouseButtonHelper(MouseButtonEvent& e) override;

        void onKeyboardHelper(KeyboardEvent& e) override;

        void onChangeThemeStyleHelper(const ThemeStyle& style) override;

        //------------------------------------------------------------------
        // Label
        //------------------------------------------------------------------

        void onTextLayoutChangeHelper() override;
    };
}
