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

    protected:
        Optional<Wstring> normalizeRawText(WstrRefer in) override;

    public:
        bool editable = true;

        void setText(WstrRefer text) override;

    protected:
        MaskObject m_visibleTextMask = {};

        D2D1_RECT_F m_visibleTextRect = {};

    public:
        const D2D1_RECT_F& visibleTextRect() const;
        void setVisibleTextRect(const D2D1_RECT_F& rect);

    protected:
        SharedPtr<Label> m_placeholder = {};

    public:
        const SharedPtr<Label>& placeholder() const;

    protected:
        D2D1_POINT_2F m_textContentOffset = {};

        // Override to take m_textContentOffset into consideration.
        size_t hitTestCaretPosition(const D2D1_POINT_2F& sfpt) override;

    protected:
        virtual D2D1_POINT_2F validateTextContentOffset(const D2D1_POINT_2F& in);

    public:
        // Override to take m_textContentOffset into consideration.
        void setCaretPosition(size_t position) override;

    public:
        virtual void performCommandCutSelection();
        virtual void performCommandPasteSelection();

    public:
        void setSelectedText(WstrRefer text) override;

    protected:
        void editSelectedText(WstrRefer text);

    protected:
        // IDrawObject2D
        void onRendererDrawD2d1LayerHelper(Renderer* rndr) override;

        void onRendererDrawD2d1ObjectHelper(Renderer* rndr) override;

        // Panel
        void onSizeHelper(SizeEvent& e) override;

        void onChangeThemeStyleHelper(const ThemeStyle& style) override;

        void onKeyboardHelper(KeyboardEvent& e) override;

    public:
        // TextInputObject
        Optional<LOGFONT> getCompositionFont() const override;
        Optional<COMPOSITIONFORM> getCompositionForm() const override;

    protected:
        void onTextInputHelper(WstrViewRefer text) override;
    };
}
