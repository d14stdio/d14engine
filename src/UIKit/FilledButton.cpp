﻿#include "Common/Precompile.h"

#include "UIKit/FilledButton.h"

#include "UIKit/IconLabel.h"
#include "UIKit/ResourceUtils.h"

namespace d14engine::uikit
{
    FilledButton::FilledButton(
        ShrdPtrRefer<IconLabel> content,
        float roundRadius,
        const D2D1_RECT_F& rect)
        :
        Panel(rect, resource_utils::solidColorBrush()),
        FlatButton(content, roundRadius, rect) { }

    FilledButton::FilledButton(
        WstrRefer text,
        float roundRadius,
        const D2D1_RECT_F& rect)
        :
        FilledButton(
            IconLabel::uniformLayout(text),
            roundRadius,
            rect) { }

    void FilledButton::onChangeThemeStyleHelper(const ThemeStyle& style)
    {
        Button::onChangeThemeStyleHelper(style);

        appearance().changeTheme(FlatButton::appearance(), style.name);
    }
}
