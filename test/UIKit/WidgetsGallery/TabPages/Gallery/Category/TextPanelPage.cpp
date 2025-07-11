﻿#include "Common/Precompile.h"

#include "TextPanelPage.h"

#include "Common/DirectXError.h"

#include "UIKit/IconLabel.h"
#include "UIKit/Label.h"

using namespace d14engine;
using namespace d14engine::uikit;

#include "PageTemplate.h"

#include "IconLabelPage.h"
#include "LabelAreaPage.h"
#include "LabelPage.h"
#include "NumberBoxPage.h"
#include "TextBoxPage.h"
#include "TextEditorPage.h"

SharedPtr<TreeViewItem> createTextPanelPage
(std::unordered_map<Wstring, SharedPtr<ConstraintLayout>>& pages)
{
    auto& ui_layout = (pages[L"Text Panel"] = makeUIObject<ConstraintLayout>());
    {
        ui_layout->maximalWidthHint = 1200.0f;
        ui_layout->setSize(920.0f, 100.0f);

        auto& appear = ui_layout->appearance();
        appear.background.opacity = 1.0f;

        auto& light = appear.g_themeData.at(L"Light");
        {
            light.background.color = D2D1::ColorF{ 0xf9f9f9 };
        }
        auto& dark = appear.g_themeData.at(L"Dark");
        {
            dark.background.color = D2D1::ColorF{ 0x272727 };
        }
    }
    auto ui_synopsisLabel = makeUIObject<Label>
    (
        L"Text panel is used to display a series of texts. "
        L"You can simply use label(area) to display static (read-only) texts, "
        L"or more advancedly, use text-box/editor to make dynamic (editable) texts."
    );
    {
        ui_synopsisLabel->setTextFormat(D14_FONT(L"Default/14"));
        ui_synopsisLabel->hardAlignment.vert = Label::VertAlignment::Top;
        THROW_IF_FAILED(ui_synopsisLabel->textLayout()->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP));

        ConstraintLayout::GeometryInfo geoInfo = {};

        ui_synopsisLabel->appearance().background = { D2D1::ColorF{ 0xff0000 }, 1.0f };

        geoInfo.keepWidth = false;
        geoInfo.Left.ToLeft = 70.0f;
        geoInfo.Right.ToRight = 70.0f;

        geoInfo.keepHeight = false;
        geoInfo.Top.ToTop = 20.0f;
        geoInfo.Bottom.ToTop = 20.0f;

        ui_layout->addElement(ui_synopsisLabel, geoInfo);
    }
    DEF_CATEGORY_ITEM(MASTER, TextPanel, Text Panel, 26X26)
    START_ADD_CHILD_ITEMS
    ADD_CHILD_ITEM_PAGE(Label, Label),
    ADD_CHILD_ITEM_PAGE(IconLabel, Icon Label),
    ADD_CHILD_ITEM_PAGE(LabelArea, Label Area),
    ADD_CHILD_ITEM_PAGE(TextBox, Text Box),
    ADD_CHILD_ITEM_PAGE(TextEditor, Text Editor),
    ADD_CHILD_ITEM_PAGE(NumberBox, Number Box)
    END_ADD_CHILD_ITEMS
    RET_CATEGORY_ITEM
}
