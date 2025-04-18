﻿#include "Common/Precompile.h"

#include <cstdlib>
#include <ctime>
#include <mutex>

#include "Common/DirectXError.h"

#include "Renderer/Renderer.h"
#include "Renderer/TickTimer.h"

#include "UIKit/AppEntry.h"
#include "UIKit/Application.h"
#include "UIKit/BitmapUtils.h"
#include "UIKit/Button.h"
#include "UIKit/Cursor.h"
#include "UIKit/ElevatedButton.h"
#include "UIKit/FilledButton.h"
#include "UIKit/FlatButton.h"
#include "UIKit/GridLayout.h"
#include "UIKit/IconLabel.h"
#include "UIKit/Label.h"
#include "UIKit/MainWindow.h"
#include "UIKit/OnOffSwitch.h"
#include "UIKit/OutlinedButton.h"
#include "UIKit/ToggleButton.h"

using namespace d14engine;
using namespace d14engine::renderer;
using namespace d14engine::uikit;

#define D14_DEMO_NAME L"ButtonFamily"

#define D14_MAINWINDOW_TITLE L"D14Engine - " D14_DEMO_NAME L" @ UIKit"
#define D14_SCREENSHOT_PATH L"Screenshots/" D14_DEMO_NAME L".png"

D14_SET_APP_ENTRY(mainButtonFamily)
{
    Application::CreateInfo info =
    {
        .windowSize = { 800, 600 }
    };
    return Application(info).run([](Application* app)
    {
        auto ui_mainWindow = makeRootUIObject<MainWindow>(D14_MAINWINDOW_TITLE);
        {
            ui_mainWindow->bringToFront();
            ui_mainWindow->maximizeButtonEnabled = false;

            ui_mainWindow->caption()->transform(300.0f, 0.0f, 376.0f, 32.0f);
        }
        auto ui_darkModeLabel = makeRootUIObject<Label>(L"Dark Mode");
        auto ui_darkModeSwitch = makeRootUIObject<OnOffSwitch>();
        {
            ui_darkModeLabel->bringToFront();
            ui_darkModeLabel->transform(10.0f, 0.0f, 120.0f, 32.0f);

            ui_darkModeSwitch->bringToFront();
            ui_darkModeSwitch->setPosition(130.0f, 4.0f);

            if (app->themeStyle().name == L"Light")
            {
                ui_darkModeSwitch->setOnOffState(OnOffSwitch::Off);
            }
            else ui_darkModeSwitch->setOnOffState(OnOffSwitch::On);

            app->f_onSystemThemeStyleChange = [app]
            (const Application::ThemeStyle& style)
            {
                app->setThemeStyle(style);
            };
            ui_darkModeSwitch->f_onStateChange = [app]
            (OnOffSwitch::StatefulObject* obj, OnOffSwitch::StatefulObject::Event& e)
            {
                Application::ThemeStyle style = app->themeStyle();
                if (e.on()) style.name = L"Dark";
                else if (e.off()) style.name = L"Light";
                app->setThemeStyle(style);
            };
        }
        auto ui_screenshot = makeRootUIObject<OutlinedButton>(L"Screenshot");
        {
            ui_screenshot->bringToFront();
            ui_screenshot->transform(200.0f, 4.0f, 100.0f, 24.0f);
            ui_screenshot->content()->label()->setTextFormat(D14_FONT(L"Default/12"));

            ui_screenshot->f_onMouseButtonRelease = [app]
            (ClickablePanel* clkp, ClickablePanel::Event& e)
            {
                auto image = app->windowshot();
                CreateDirectory(L"Screenshots", nullptr);
                bitmap_utils::saveBitmap(image.Get(), D14_SCREENSHOT_PATH);
            };
        }
        auto ui_centerLayout = makeUIObject<GridLayout>();
        {
            ui_centerLayout->appearance().background.opacity = 1.0f;
            ui_centerLayout->setCellCount(2, 3);

            ui_mainWindow->setContent(ui_centerLayout);
        }
        auto ui_button = makeUIObject<Button>(L"Click to change count");
        {
            ui_button->setSize(200.0f, 50.0f);

            GridLayout::GeometryInfo geoInfo = {};
            geoInfo.isFixedSize = true;
            geoInfo.axis.x = { 0, 1 };
            geoInfo.axis.y = { 0, 1 };
            ui_centerLayout->addElement(ui_button, geoInfo);

            auto ui_label1 = makeManagedUIObject<Label>(ui_centerLayout, L"Current count: 0");
            ui_label1->transform(math_utils::offset(ui_button->relativeRect(), { 0.0f, -60.0f }));
            THROW_IF_FAILED(ui_label1->textLayout()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));

            auto ui_label2 = makeManagedUIObject<Label>(ui_centerLayout);
            ui_label2->transform(math_utils::offset(ui_button->relativeRect(), { 0.0f, 60.0f }));
            ui_label2->setText(L"Left ++ , Right --\nMiddle 0 , Wheel Change");
            THROW_IF_FAILED(ui_label2->textLayout()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));

            auto currCount = std::make_shared<int>(0);
            auto wk_label1 = (WeakPtr<Label>)ui_label1;

            ui_button->f_onMouseButtonRelease = [=]
            (ClickablePanel* p, ClickablePanel::Event& e)
            {
                if (e.left()) ++(*currCount);
                else if (e.right()) --(*currCount);
                else if (e.middle()) (*currCount) = 0;

                if (!wk_label1.expired())
                {
                    auto sh_label1 = wk_label1.lock();
                    sh_label1->setText(L"Current count: " + std::to_wstring(*currCount));
                }
            };
            ui_button->f_onMouseWheel = [=](Panel* p, MouseWheelEvent& e)
            {
                (*currCount) += e.deltaCount;

                if (!wk_label1.expired())
                {
                    auto sh_label1 = wk_label1.lock();
                    sh_label1->setText(L"Current count: " + std::to_wstring(*currCount));
                }
            };
        }
        auto ui_toggleButton = makeUIObject<ToggleButton>(L"Play Animation");
        {
            ui_toggleButton->setSize(200.0f, 50.0f);

            GridLayout::GeometryInfo geoInfo = {};
            geoInfo.isFixedSize = true;
            geoInfo.axis.x = { 1, 1 };
            geoInfo.axis.y = { 0, 1 };
            ui_centerLayout->addElement(ui_toggleButton, geoInfo);

            auto ui_label1 = makeManagedUIObject<Label>(ui_centerLayout, L"Animation Mode: Off");
            ui_label1->transform(math_utils::offset(ui_toggleButton->relativeRect(), { 0.0f, -60.0f }));
            THROW_IF_FAILED(ui_label1->textLayout()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));

            auto ui_label2 = makeManagedUIObject<Label>(ui_centerLayout, L"Current cursor: Arrow");
            ui_label2->transform(math_utils::offset(ui_toggleButton->relativeRect(), { 0.0f, 60.0f }));
            THROW_IF_FAILED(ui_label2->textLayout()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));

            auto useDynamicCursor = std::make_shared<bool>(false);
            auto wk_label1 = (WeakPtr<Label>)ui_label1;
            auto wk_label2 = (WeakPtr<Label>)ui_label2;

            ui_centerLayout->f_onMouseMove = [=](Panel* p, MouseMoveEvent& e)
            {
                if (*useDynamicCursor) app->cursor()->setIcon(Cursor::Working);
                else app->cursor()->setIcon(Cursor::Arrow);
            };
            ui_toggleButton->f_onStateChange = [=]
            (ToggleButton::StatefulObject* obj, ToggleButton::StatefulObject::Event& e)
            {
                if (!wk_label1.expired() && !wk_label2.expired())
                {
                    auto sh_label1 = wk_label1.lock();
                    auto sh_label2 = wk_label2.lock();

                    if (e.activated())
                    {
                        app->increaseAnimationCount();

                        *useDynamicCursor = true;
                        app->cursor()->setIcon(Cursor::Working);

                        sh_label1->setText(L"FPS: 0");
                        sh_label2->setText(L"Current cursor: Working");
                    }
                    else if (e.deactivated())
                    {
                        app->decreaseAnimationCount();

                        *useDynamicCursor = false;
                        app->cursor()->setIcon(Cursor::Arrow);

                        sh_label1->setText(L"Animation Mode: Off");
                        sh_label2->setText(L"Current cursor: Arrow");
                    }
                }
            };
            ui_toggleButton->f_onRendererUpdateObject2DAfter = [=](Panel* p, Renderer* rndr)
            {
                if (!wk_label1.expired() && *useDynamicCursor)
                {
                    auto sh_label1 = wk_label1.lock();
                    sh_label1->setText(L"FPS: " + std::to_wstring(rndr->timer()->fpsNum()));
                }
            };
        }
        auto ui_flatButton = makeUIObject<FlatButton>(L"(+/-) X-radius");
        auto ui_filledButton = makeUIObject<FilledButton>(L"Random X-radius");
        auto ui_xRadLabel = makeManagedUIObject<Label>(ui_centerLayout);
        {
            ui_flatButton->setSize(200.0f, 50.0f);

            GridLayout::GeometryInfo geoInfo1 = {};
            geoInfo1.isFixedSize = true;
            geoInfo1.axis.x = { 0, 1 };
            geoInfo1.axis.y = { 1, 1 };
            ui_centerLayout->addElement(ui_flatButton, geoInfo1);

            ui_filledButton->setSize(200.0f, 50.0f);

            GridLayout::GeometryInfo geoInfo2 = {};
            geoInfo2.isFixedSize = true;
            geoInfo2.axis.x = { 0, 1 };
            geoInfo2.axis.y = { 2, 1 };
            ui_centerLayout->addElement(ui_filledButton, geoInfo2);

            ui_xRadLabel->transform(math_utils::offset(ui_flatButton->relativeRect(), { 0.0f, 85.0f }));
            ui_xRadLabel->setText(L"Round radius X: 0px\n(Try wheel above this)");
            THROW_IF_FAILED(ui_xRadLabel->textLayout()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));
        }
        auto ui_outlinedButton = makeUIObject<OutlinedButton>(L"(+/-) Y-radius");
        auto ui_elevatedButton = makeUIObject<ElevatedButton>(L"Random Y-radius");
        auto ui_yRadLabel = makeManagedUIObject<Label>(ui_centerLayout);
        {
            ui_outlinedButton->setSize(200.0f, 50.0f);

            GridLayout::GeometryInfo geoInfo1 = {};
            geoInfo1.isFixedSize = true;
            geoInfo1.axis.x = { 1, 1 };
            geoInfo1.axis.y = { 1, 1 };
            ui_centerLayout->addElement(ui_outlinedButton, geoInfo1);

            ui_elevatedButton->setSize(200.0f, 50.0f);

            GridLayout::GeometryInfo geoInfo2 = {};
            geoInfo2.isFixedSize = true;
            geoInfo2.axis.x = { 1, 1 };
            geoInfo2.axis.y = { 2, 1 };
            ui_centerLayout->addElement(ui_elevatedButton, geoInfo2);

            ui_yRadLabel->transform(math_utils::offset(ui_outlinedButton->relativeRect(), { 0.0f, 85.0f }));
            ui_yRadLabel->setText(L"Round radius Y: 0px\n(Try wheel above this)");
            THROW_IF_FAILED(ui_yRadLabel->textLayout()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));
        }
        // Set add/reduce/random round-radius callbacks.
        {
            std::array<WeakPtr<Button>, 6> wk_buttons =
            {
                ui_button, ui_toggleButton,
                ui_flatButton, ui_filledButton,
                ui_outlinedButton, ui_elevatedButton
            };
            auto wk_labelX = (WeakPtr<Label>)ui_xRadLabel;
            auto wk_labelY = (WeakPtr<Label>)ui_yRadLabel;

            ui_flatButton->f_onMouseButtonRelease = [=]
            (ClickablePanel* p, ClickablePanel::Event& e)
            {
                int roundRadiusX = 0;
                for (auto& wk_button : wk_buttons)
                {
                    if (!wk_button.expired())
                    {
                        auto sh_button = wk_button.lock();
                        if (e.left())
                        {
                            roundRadiusX = (int)++sh_button->roundRadiusX;
                        }
                        if (e.right())
                        {
                            roundRadiusX = (int)--sh_button->roundRadiusX;
                        }
                    }
                }
                if (!wk_labelX.expired())
                {
                    wk_labelX.lock()->setText(
                        L"Round radius X: " + std::to_wstring(roundRadiusX) +
                        L"px" + L"\n(Try wheel above this)");
                }
            };
            static int roundRadiusX = 0;

            static auto idx = (UINT64)&roundRadiusX;
            static std::mutex mxx = {};

            ui_filledButton->f_onMouseButtonRelease = [=]
            (ClickablePanel* p, ClickablePanel::Event& e)
            {
                // Test Multi-threading Functionality
                Thread([=]
                {
                    srand((unsigned int)time(0));
                    mxx.lock();
                    roundRadiusX = rand() % 30;
                    mxx.unlock();
                    app->triggerThreadEvent(idx);
                })
                .detach();
            };
            app->registerThreadCallback(idx, [=](auto data)
            {
                for (auto& wk_button : wk_buttons)
                {
                    if (!wk_button.expired())
                    {
                        auto sh_button = wk_button.lock();
                        sh_button->roundRadiusX = (float)roundRadiusX;
                    }
                }
                if (!wk_labelX.expired())
                {
                    wk_labelX.lock()->setText(
                        L"Round radius X: " + std::to_wstring(roundRadiusX) +
                        L"px" + L"\n(Try wheel above this)");
                }
            });
            ui_outlinedButton->f_onMouseButtonRelease = [=]
            (ClickablePanel* p, ClickablePanel::Event& e)
            {
                int roundRadiusY = 0;
                for (auto& wk_button : wk_buttons)
                {
                    if (!wk_button.expired())
                    {
                        auto sh_button = wk_button.lock();
                        if (e.left())
                        {
                            roundRadiusY = (int)++sh_button->roundRadiusY;
                        }
                        if (e.right())
                        {
                            roundRadiusY = (int)--sh_button->roundRadiusY;
                        }
                    }
                }
                if (!wk_labelY.expired())
                {
                    wk_labelY.lock()->setText(
                        L"Round radius Y: " + std::to_wstring(roundRadiusY) +
                        L"px" + L"\n(Try wheel above this)");
                }
            };
            static int roundRadiusY = 0;

            static auto idy = (UINT64)&roundRadiusY;
            static std::mutex mxy = {};

            ui_elevatedButton->f_onMouseButtonRelease = [=]
            (ClickablePanel* p, ClickablePanel::Event& e)
            {
                // Test Multi-threading Functionality
                Thread([=]
                {
                    srand((unsigned int)time(0));
                    mxy.lock();
                    roundRadiusY = rand() % 30;
                    mxy.unlock();
                    app->triggerThreadEvent(idy);
                })
                .detach();
            };
            app->registerThreadCallback(idy, [=](auto data)
            {
                for (auto& wk_button : wk_buttons)
                {
                    if (!wk_button.expired())
                    {
                        auto sh_button = wk_button.lock();
                        sh_button->roundRadiusY = (float)roundRadiusY;
                    }
                }
                if (!wk_labelY.expired())
                {
                    wk_labelY.lock()->setText(
                        L"Round radius Y: " + std::to_wstring(roundRadiusY) +
                        L"px" + L"\n(Try wheel above this)");
                }
            });
            ui_xRadLabel->f_onMouseWheel = [=](Panel* p, MouseWheelEvent& e)
            {
                int roundRadiusX = 0;
                for (auto& wk_button : wk_buttons)
                {
                    if (!wk_button.expired())
                    {
                        auto sh_button = wk_button.lock();
                        roundRadiusX = (int)(sh_button->roundRadiusX += e.deltaCount);
                    }
                }
                ((Label*)p)->setText(
                    L"Round radius X: " + std::to_wstring(roundRadiusX) +
                    L"px" + L"\n(Try wheel above this)");
            };
            ui_yRadLabel->f_onMouseWheel = [=](Panel* p, MouseWheelEvent& e)
            {
                int roundRadiusY = 0;
                for (auto& wk_button : wk_buttons)
                {
                    if (!wk_button.expired())
                    {
                        auto sh_button = wk_button.lock();
                        roundRadiusY = (int)(sh_button->roundRadiusY += e.deltaCount);
                    }
                }
                ((Label*)p)->setText(
                    L"Round radius Y: " + std::to_wstring(roundRadiusY) +
                    L"px" + L"\n(Try wheel above this)");
            };
        }
    });
}
