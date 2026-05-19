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
        void onTextInput(WstrViewParam text)
        {
            onTextInputHelper(text);

            if (f_onTextInput) f_onTextInput(this, text);
        }
        Function<void(TextInputObject*, WstrViewParam)> f_onTextInput = {};

        void onTextChanged(WstrParam text)
        {
            onTextChangedHelper(text);

            if (f_onTextChanged) f_onTextChanged(this, text);
        }
        Function<void(TextInputObject*, WstrParam)> f_onTextChanged = {};

        void onTextEdited(WstrParam text)
        {
            onTextEditedHelper(text);

            if (f_onTextEdited) f_onTextEdited(this, text);
        }
        Function<void(TextInputObject*, WstrParam)> f_onTextEdited = {};

    protected:
        virtual void onTextInputHelper(WstrViewParam text)
        {
            // This method intentionally left blank.
        }
        virtual void onTextChangedHelper(WstrParam text)
        {
            // This method intentionally left blank.
        }
        virtual void onTextEditedHelper(WstrParam text)
        {
            // This method intentionally left blank.
        }
    };
}
