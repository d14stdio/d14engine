#pragma once

#include "Common/Precompile.h"

#include "UIKit/Appearances/RawTextInput.h"
#include "UIKit/LabelArea.h"
#include "UIKit/MaskObject.h"
#include "UIKit/TextInputObject.h"

namespace d14engine::uikit
{
    struct RawTextInput : appearance::RawTextInput, LabelArea, TextInputObject
    {
        RawTextInput(bool multiline, float roundRadius = 0.0f, const D2D1_RECT_F& rect = {});

        void initialize() override;

        _D14_SET_APPEARANCE_PROPERTY(RawTextInput)

        const bool multiline = {};

        ////////////////////////
        // Callback Functions //
        ////////////////////////

        //------------------------------------------------------------------
        // Public Interfaces
        //------------------------------------------------------------------
    public:
        void onTextContentOffsetChange(const D2D1_POINT_2F& offset);

        Function<void(RawTextInput*, const D2D1_POINT_2F&)> f_onTextContentOffsetChange = {};

        //------------------------------------------------------------------
        // Protected Helpers
        //------------------------------------------------------------------
    protected:
        virtual void onTextContentOffsetChangeHelper(const D2D1_POINT_2F& offset);

        //////////////////////////
        // Graphical Components //
        //////////////////////////

    public:
        bool editable = true;

        //------------------------------------------------------------------
        // Visible Text Area
        //------------------------------------------------------------------
    protected:
        MaskObject m_visibleTextMask = {};

        D2D1_RECT_F m_visibleTextRect = {};

    public:
        const D2D1_RECT_F& visibleTextRect() const;
        void setVisibleTextRect(const D2D1_RECT_F& rect);

        //------------------------------------------------------------------
        // Text Content Offset
        //------------------------------------------------------------------
    protected:
        D2D1_POINT_2F m_textContentOffset = {};

    protected:
        virtual D2D1_POINT_2F validateTextContentOffset(const D2D1_POINT_2F& in);

    public:
        const D2D1_POINT_2F& textContentOffset() const;
        void setTextContentOffset(const D2D1_POINT_2F& offset);

        // Update the offset without triggering the corresponding event.
        void setTextContentOffsetSilently(const D2D1_POINT_2F& offset);

        //------------------------------------------------------------------
        // Placeholder
        //------------------------------------------------------------------
    protected:
        SharedPtr<Label> m_placeholder = {};

    public:
        const SharedPtr<Label>& placeholder() const;

        ///////////////////////
        // Interaction Logic //
        ///////////////////////

        //------------------------------------------------------------------
        // Text Editing
        //------------------------------------------------------------------
    protected:
        void editSelectedText(WstrParam text);

    public:
        virtual void performCommandCutSelection();
        virtual void performCommandPasteSelection();

        /////////////////////////
        // Interface Overrides //
        /////////////////////////

    public:
        //------------------------------------------------------------------
        // TextInputObject
        //------------------------------------------------------------------

        Optional<LOGFONT> getCompositionFont() const override;
        Optional<COMPOSITIONFORM> getCompositionForm() const override;

        //------------------------------------------------------------------
        // Label
        //------------------------------------------------------------------

        void setText(WstrParam text) override;

        //------------------------------------------------------------------
        // LabelArea
        //------------------------------------------------------------------

        void setCaretPosition(size_t position) override;

        void setSelectedText(WstrParam text) override;

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

        void onKeyboardHelper(KeyboardEvent& e) override;

        //------------------------------------------------------------------
        // TextInputObject
        //------------------------------------------------------------------

        void onTextInputHelper(WstrViewParam text) override;

        //------------------------------------------------------------------
        // Label
        //------------------------------------------------------------------

        Optional<Wstring> normalizeText(WstrParam in) override;

        //------------------------------------------------------------------
        // LabelArea
        //------------------------------------------------------------------

        size_t hitTestCaretPosition(const D2D1_POINT_2F& sfpt) override;
    };
}
