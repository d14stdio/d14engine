﻿#include "Common/Precompile.h"

#include "Common/MathUtils/GDI.h"

#include "UIKit/AppEntry.h"
#include "UIKit/Application.h"
#include "UIKit/ConstraintLayout.h"
#include "UIKit/IconLabel.h"
#include "UIKit/Label.h"
#include "UIKit/MainWindow.h"
#include "UIKit/TabCaption.h"
#include "UIKit/TabGroup.h"

using namespace d14engine;
using namespace d14engine::uikit;

#include "TabPages/Basic/Basic.h"
#include "TabPages/Settings/Settings.h"

D14_SET_APP_ENTRY(mainEditor)
{
    Application::CreateInfo info = {};
    if (argc >= 2 && lstrcmp(argv[1], L"HighDPI") == 0)
    {
        info.dpi = 192.0f;
    }
    else info.dpi = 96.0f;
    info.windowSize = { 1280, 720 };

    return Application(info).run([&](Application* app)
    {
        auto ui_mainWindow = makeRootUIObject<MainWindow>(L"D14Engine - Editor");
        {
            ui_mainWindow->bringToFront();

            ui_mainWindow->setCaptionPanelHeight(40.0f);
            ui_mainWindow->setDecorativeBarHeight(2.0f);

            ui_mainWindow->setDisplayState(Window::Maximized);
        }
        auto ui_centerLayout = makeUIObject<ConstraintLayout>();
        {
            ui_mainWindow->setContent(ui_centerLayout);
        }
        auto ui_tabGroup = makeUIObject<TabGroup>();
        {
            ConstraintLayout::GeometryInfo geoInfo = {};

            geoInfo.keepWidth = false;
            geoInfo.Left.ToLeft = 0.0f;
            geoInfo.Right.ToRight = 0.0f;

            geoInfo.keepHeight = false;
            geoInfo.Top.ToTop = 50.0f;
            geoInfo.Bottom.ToBottom = 0.0f;

            ui_centerLayout->addElement(ui_tabGroup, geoInfo);

#define SET_CARD_SIZE(State, Width, Height) \
    ui_tabGroup->appearance().tabBar.card.main \
    [(size_t)TabGroup::CardState::State].geometry.size = { Width, Height }

            SET_CARD_SIZE(Dormant, 250.0f, 32.0f);
            SET_CARD_SIZE(Hover,   250.0f, 32.0f);
            SET_CARD_SIZE(Active,  266.0f, 40.0f);

#undef SET_CARD_SIZE
            ui_tabGroup->activeCard.loadMask();
            ui_tabGroup->activeCard.loadPathGeo();

            auto& barAppear = ui_tabGroup->appearance().tabBar;

            barAppear.geometry.height = 40.0f;
            barAppear.separator.geometry.size.height = 24.0f;
            barAppear.moreCards.control.button.geometry.offset.y = 7.0f;

            ui_tabGroup->f_onTriggerTabPromoting = [](TabGroup* tg, Window* w)
            {
                w->registerDrawObjects();
                w->registerApplicationEvents();

                w->bringToFront();

                w->button1Enabled = false;
                w->button2Enabled = false;

                w->f_onClose = [](Window* w) { w->release(); };
            };
        }
        auto appendTabPage = [&](WstrRefer title)
        {
            auto ui_caption = makeUIObject<TabCaption>(title);
            auto ui_content = makeUIObject<ConstraintLayout>();

            ui_caption->title()->label()->setTextFormat(D14_FONT(L"Default/14"));

            ui_caption->promotable = true;

            ui_tabGroup->appendTab({ ui_caption, ui_content });

            return ui_content;
        };
#define SET_TAB_PAGE(Name, Title) \
        create##Name##TabPage(appendTabPage(L#Title).get())

        SET_TAB_PAGE(Basic,    Basic);
        SET_TAB_PAGE(Settings, Settings);

#undef SET_TAB_PAGE
        ui_tabGroup->selectTab(0);

        app->win32WindowSettings.geometry.minTrackSize = { 1280, 720 };

        app->win32WindowSettings.sizingFrame.frameWidth = 6;
        app->win32WindowSettings.sizingFrame.f_NCHITTEST = [=]
        (const POINT& pt) -> LRESULT
        {
            auto frmWidth = app->win32WindowSettings.sizingFrame.frameWidth.value();

            RECT clntRect = {};
            GetClientRect(app->win32Window(), &clntRect);
            auto clntSize = math_utils::size(clntRect);

            // Escape the area of tab group card bar.
            if (pt.y >= 42 && pt.y <= 92)
            {
                return HTCLIENT;
            }
            // Escape the area of main window 3 brothers.
            if (pt.y <= frmWidth && pt.x >= (clntSize.cx - 124) && pt.x <= (clntSize.cx - 20))
            {
                return HTCLIENT;
            }
            return app->defWin32NCHITTESTMessageHandler(pt);
        };
    });
}
