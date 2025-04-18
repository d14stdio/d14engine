﻿#include "Common/Precompile.h"

#include "TabGroupPage.h"

#include "UIKit/IconLabel.h"
#include "UIKit/Label.h"

using namespace d14engine;
using namespace d14engine::uikit;

#include "PageTemplate.h"

SharedPtr<TreeViewItem> createTabGroupPage(ConstraintLayout* page)
{
    // Initialize and populate the widget page with detailed content.
    {
        page->setSize(920.0f, 1000.0f);

        auto& appear = page->appearance();
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
    DEF_CATEGORY_ITEM(SLAVER, TabGroup, Tab Group, 30X18)
    RET_CATEGORY_ITEM
}
