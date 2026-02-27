#pragma once

#include "Common/Precompile.h"

namespace d14engine::uikit
{
    struct TextInputObject
    {
        //////////////////////////
        // IME Composition Form //
        //////////////////////////
    public:
        virtual Optional<LOGFONT> getCompositionFont() const
        {
            return std::nullopt;
        }
        virtual Optional<COMPOSITIONFORM> getCompositionForm() const
        {
            return std::nullopt;
        }

        //////////////////////////
        // Text Event Callbacks //
        //////////////////////////
    public:
        void onTextInput(WstrViewRefer text)
        {
            onTextInputHelper(text);

            if (f_onTextInput) f_onTextInput(this, text);
        }
        Function<void(TextInputObject*, WstrViewRefer)> f_onTextInput = {};

        void onTextChanged(WstrRefer text)
        {
            onTextChangedHelper(text);

            if (f_onTextChanged) f_onTextChanged(this, text);
        }
        Function<void(TextInputObject*, WstrRefer)> f_onTextChanged = {};

        void onTextEdited(WstrRefer text)
        {
            onTextEditedHelper(text);

            if (f_onTextEdited) f_onTextEdited(this, text);
        }
        Function<void(TextInputObject*, WstrRefer)> f_onTextEdited = {};

    protected:
        virtual void onTextInputHelper(WstrViewRefer text)
        {
            // This method intentionally left blank.
        }
        virtual void onTextChangedHelper(WstrRefer text)
        {
            // This method intentionally left blank.
        }
        virtual void onTextEditedHelper(WstrRefer text)
        {
            // This method intentionally left blank.
        }
    };
}
