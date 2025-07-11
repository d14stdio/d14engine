﻿#include "Common/Precompile.h"

#include "UIKit/AppEntry.h"
#include "UIKit/Application.h"
#include "UIKit/BitmapUtils.h"
#include "UIKit/IconLabel2.h"
#include "UIKit/MainWindow.h"
#include "UIKit/MenuSeparator.h"
#include "UIKit/OnOffSwitch.h"
#include "UIKit/OutlinedButton.h"
#include "UIKit/PopupMenu.h"
#include "UIKit/TextBox.h"
#include "UIKit/TextEditor.h"

using namespace d14engine;
using namespace d14engine::uikit;

#define D14_DEMO_NAME L"SimpleEditor"

#define D14_MAINWINDOW_TITLE L"D14Engine - " D14_DEMO_NAME L" @ UIKit"
#define D14_SCREENSHOT_PATH L"Screenshots/" D14_DEMO_NAME L".png"

D14_SET_APP_ENTRY(mainSimpleEditor)
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
        auto ui_clientArea = makeUIObject<Panel>();
        {
            ui_mainWindow->setContent(ui_clientArea);
        }
        auto ui_titleInput = makeManagedUIObject<TextBox>(ui_clientArea, 5.0f);
        {
            ui_titleInput->transform(20.0f, 20.0f, 500.0f, 46.0f);

            ui_titleInput->setVisibleTextRect({ 5.0f, 8.0f, 495.0f, 38.0f });
            ui_titleInput->placeholder()->setText(L"Title");

            ui_titleInput->f_onTextChange =
            [
                wk_mainWindow = (WeakPtr<MainWindow>)ui_mainWindow
            ]
            (TextBox::TextInputObject* obj, WstrRefer text)
            {
                if (!wk_mainWindow.expired())
                {
                    wk_mainWindow.lock()->caption()->label()->setText(text);
                }
            };
        }
        auto ui_characterCount = makeManagedUIObject<Label>(ui_clientArea);
        {
            ui_characterCount->transform(540.0f, 20.0f, 220.0f, 46.0f);

            ui_characterCount->setText(L"Character count: 0");
        }
        auto menuRect = math_utils::sizeOnlyRect({ 260.0f, 222.0f });
        auto ui_inputContextMenu = makeRootUIObject<PopupMenu>(menuRect);
        {
            ui_inputContextMenu->appendItem(
            {
                makeUIObject<MenuItem>(IconLabel2::menuItemLayout
                (
                    L"Select all", L"Ctrl+A"), math_utils::heightOnlyRect(40.0f)
                ),
                makeUIObject<MenuSeparator>(math_utils::heightOnlyRect(11.0f)),

                makeUIObject<MenuItem>(IconLabel2::menuItemLayout
                (
                    L"Cut", L"Ctrl+X"), math_utils::heightOnlyRect(40.0f)
                ),
                makeUIObject<MenuItem>(IconLabel2::menuItemLayout
                (
                    L"Copy", L"Ctrl+C"), math_utils::heightOnlyRect(40.0f)
                ),
                makeUIObject<MenuItem>(IconLabel2::menuItemLayout
                (
                    L"Paste", L"Ctrl+V"), math_utils::heightOnlyRect(40.0f)
                ),
                makeUIObject<MenuSeparator>(math_utils::heightOnlyRect(11.0f)),

                makeUIObject<MenuItem>(IconLabel2::menuItemLayout
                (
                    L"Editable", L"✓"), math_utils::heightOnlyRect(40.0f)
                )
            });
            ui_inputContextMenu->items().back()->isInstant = false;

            ui_inputContextMenu->constrainedRectangle = { 0.0f, 0.0f, 800.0f, 600.0f };

            ui_inputContextMenu->setBackgroundTriggerPanel(true);
        }
        auto ui_mainBodyView = makeManagedUIObject<ScrollView>(ui_clientArea);
        auto ui_mainBodyInput = makeManagedUIObject<TextEditor>(ui_mainBodyView, 5.0f);
        {
            ui_mainBodyView->transform(25.0f, 94.0f, 767.0f, 462.0f);

            ui_mainBodyView->customSizeGetter.view =
            [
                wk_mainBodyInput = (WeakPtr<TextEditor>)ui_mainBodyInput
            ]
            (const ScrollView* sv) -> D2D1_SIZE_F
            {
                if (wk_mainBodyInput.expired()) return { 0.0f, 0.0f };
                return math_utils::size(wk_mainBodyInput.lock()->visibleTextRect());
            };
            ui_mainBodyView->customSizeGetter.content =
            [
                wk_mainBodyInput = (WeakPtr<TextEditor>)ui_mainBodyInput
            ]
            (const ScrollView* sv) -> D2D1_SIZE_F
            {
                if (wk_mainBodyInput.expired()) return { 0.0f, 0.0f };
                auto metrics = wk_mainBodyInput.lock()->textMetrics();
                return { metrics.widthIncludingTrailingWhitespace, metrics.height };
            };
            ui_mainBodyView->f_onViewportOffsetChange =
            [
                wk_mainBodyInput = (WeakPtr<TextEditor>)ui_mainBodyInput
            ]
            (ScrollView* sv, const D2D1_POINT_2F& offset)
            {
                if (!wk_mainBodyInput.expired())
                {
                    wk_mainBodyInput.lock()->setTextContentOffset(offset);
                }
            };
            ui_mainBodyInput->transform( -5.0f, -8.0f, 760.0f, 458.0f);

            ui_mainBodyInput->keepIndicatorPosition = true;

            ui_mainBodyInput->f_onParentSize = [](Panel* p, SizeEvent& e)
            {
                p->setSize({ e.size.width + 10.0f, e.size.height + 16.0f });
            };
            ui_mainBodyInput->setVisibleTextRect({ 5.0f, 8.0f, 755.0f, 450.0f });
            ui_mainBodyInput->placeholder()->setText(L"Main body");

            ui_mainBodyInput->f_onMouseButton =
            [
                wk_inputContextMenu = (WeakPtr<PopupMenu>)ui_inputContextMenu
            ]
            (Panel* p, MouseButtonEvent& e)
            {
                if (!wk_inputContextMenu.expired() && e.state.rightUp())
                {
                    auto sh_inputContextMenu = wk_inputContextMenu.lock();
                    sh_inputContextMenu->showInConstrainedRect(e.cursorPoint);
                }
            };
            ui_mainBodyInput->f_onTextChange =
            [
                wk_characterCount = (WeakPtr<Label>)ui_characterCount
            ]
            (TextEditor::TextInputObject* obj, WstrRefer text)
            {
                if (!wk_characterCount.expired())
                {
                    auto count = ((TextEditor*)obj)->text().size();
                    wk_characterCount.lock()->setText(
                        L"Character count: " + std::to_wstring(count));
                }
            };
            ui_mainBodyInput->f_onTextContentOffsetChange =
            [
                wk_mainBodyView = (WeakPtr<ScrollView>)ui_mainBodyView
            ]
            (RawTextInput* rti, const D2D1_POINT_2F& offset)
            {
                if (!wk_mainBodyView.expired())
                {
                    wk_mainBodyView.lock()->setViewportOffset(offset);
                }
            };
            ui_inputContextMenu->f_onTriggerMenuItem =
            [
                wk_mainBodyInput = (WeakPtr<TextEditor>)ui_mainBodyInput
            ]
            (PopupMenu* menu, size_t index)
            {
                if (!wk_mainBodyInput.expired())
                {
                    auto sh_mainBodyInput = wk_mainBodyInput.lock();
                    switch (index)
                    {
                    case 0: sh_mainBodyInput->performCommandCtrlA(); break;
                    case 2: sh_mainBodyInput->performCommandCtrlX(); break;
                    case 3: sh_mainBodyInput->performCommandCtrlC(); break;
                    case 4: sh_mainBodyInput->performCommandCtrlV(); break;
                    case 6:
                    {
                        auto& editable = sh_mainBodyInput->editable;

                        editable = !editable;
                        auto content = menu->items()[index]->getContent<IconLabel2>();
                        if (!content.expired())
                        {
                            content.lock()->label2()->setText(
                                editable ? L"✓" : L"");
                        }
                        // Remember to disable the cut/paste if not editable.
                        menu->items()[2]->setEnabled(editable);
                        menu->items()[4]->setEnabled(editable);
                        break;
                    }
                    default: break;
                    }
                }
            };
        }
    });
}
