﻿#include "Common/Precompile.h"

#include "Renderer/Renderer.h"
#include "Renderer/TickTimer.h"

#include "UIKit/AppEntry.h"
#include "UIKit/Application.h"
#include "UIKit/BitmapUtils.h"
#include "UIKit/FileSystemUtils.h"
#include "UIKit/FrameAnimPanel.h"
#include "UIKit/GridLayout.h"
#include "UIKit/HorzSlider.h"
#include "UIKit/IconLabel.h"
#include "UIKit/Label.h"
#include "UIKit/MainWindow.h"
#include "UIKit/OnOffSwitch.h"
#include "UIKit/OutlinedButton.h"
#include "UIKit/RawTextBox.h"
#include "UIKit/ResourceUtils.h"
#include "UIKit/TabCaption.h"
#include "UIKit/TabGroup.h"
#include "UIKit/ToggleButton.h"

using namespace d14engine;
using namespace d14engine::renderer;
using namespace d14engine::uikit;

#define D14_DEMO_NAME L"PixelAnimation"

#define D14_MAINWINDOW_TITLE L"D14Engine - " D14_DEMO_NAME L" @ UIKit"
#define D14_SCREENSHOT_PATH L"Screenshots/" D14_DEMO_NAME L".png"

D14_SET_APP_ENTRY(mainPixelAnimation)
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
        auto ui_pixelViewer = makeManagedUIObject<TabGroup>(ui_clientArea);
        auto wk_pixelViewer = (WeakPtr<TabGroup>)ui_pixelViewer;
        {
            ui_pixelViewer->transform(0.0f, 40.0f, 500.0f, 524.0f);
        }
        auto ui_stickBoy = makeUIObject<FrameAnimPanel>();
        auto wk_stickBoy = (WeakPtr<FrameAnimPanel>)ui_stickBoy;
        {
            ui_stickBoy->setSize(256.0f, 256.0f);

            Wstring assetsPath = L"test/UIKit/PixelAnimation/stick_boy/";
            animation_utils::BitmapSequence::FramePackage rawFrames = {};

            file_system_utils::foreachFileInDir(assetsPath, L"*.png", [&](WstrRefer& path)
            {
                auto index = file_system_utils::extractFilePrefix(
                            file_system_utils::extractFileName(path));

                BitmapObject bmpobj = bitmap_utils::loadBitmap(path);
                bmpobj.interpolationMode = D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR;

                rawFrames.insert({ index, bmpobj }); return false;
            });
            auto& fanim = ui_stickBoy->bitmapData.fanim;
            fanim.frames.resize(rawFrames.size());
            for (auto& kv : rawFrames)
            {
                fanim.frames[std::stoi(kv.first)] = kv.second;
                fanim.timeSpanDataInSecs = 0.06f;
            }
            auto caption = makeUIObject<TabCaption>(L"stick_boy");
            caption->title()->label()->setTextFormat(D14_FONT(L"Default/12"));

            auto content = makeUIObject<GridLayout>();
            GridLayout::GeometryInfo geoInfo = {};
            geoInfo.isFixedSize = true;
            geoInfo.axis.x = { 0, 1 };
            geoInfo.axis.y = { 0, 1 };
            content->addElement(ui_stickBoy, geoInfo);

            ui_pixelViewer->appendTab({{ caption, content }});
            ui_pixelViewer->setSelectedTab(0);

            for (int i = 0; i < fanim.frames.size(); ++i)
            {
                auto ui_stickBoyFrame = makeUIObject<Panel>();
                ui_stickBoyFrame->setSize(256.0f, 256.0f);
                ui_stickBoyFrame->bitmap = fanim.frames[i].data;
                auto& interpMode = ui_stickBoyFrame->bitmapProperty.interpolationMode;
                interpMode = D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR;

                auto caption = makeUIObject<TabCaption>(std::to_wstring(i));
                caption->title()->label()->setTextFormat(D14_FONT(L"Default/12"));

                auto content = makeUIObject<GridLayout>();
                GridLayout::GeometryInfo geoInfo = {};
                geoInfo.isFixedSize = true;
                geoInfo.axis.x = { 0, 1 };
                geoInfo.axis.y = { 0, 1 };
                content->addElement(ui_stickBoyFrame, geoInfo);

                ui_pixelViewer->appendTab({{ caption, content }});
            }
        }
        auto ui_sideLayout = makeManagedUIObject<GridLayout>(ui_clientArea);
        {
            ui_sideLayout->transform(500.0f, 0.0f, 300.0f, 564.0f);
            ui_sideLayout->setCellCount(5, 8);
        }
        auto ui_fpsLabel = makeUIObject<Label>(L"FPS: None");
        auto wk_fpsLabel = (WeakPtr<Label>)ui_fpsLabel;
        auto ui_animCtrlButton = makeUIObject<ToggleButton>(L"Start Anim");
        auto wk_animCtrlButton = (WeakPtr<ToggleButton>)ui_animCtrlButton;
        {
            ui_fpsLabel->setSize(250.0f, 50.0f);

            ui_animCtrlButton->roundRadiusX =
            ui_animCtrlButton->roundRadiusY = 8.0f;
            ui_animCtrlButton->setSize(250.0f, 50.0f);

            GridLayout::GeometryInfo geoInfo1 = {};
            geoInfo1.isFixedSize = true;
            geoInfo1.axis.x = { 0, 5 };
            geoInfo1.axis.y = { 0, 1 };
            ui_sideLayout->addElement(ui_fpsLabel, geoInfo1);

            GridLayout::GeometryInfo geoInfo2 = {};
            geoInfo2.isFixedSize = true;
            geoInfo2.axis.x = { 0, 5 };
            geoInfo2.axis.y = { 1, 1 };
            ui_sideLayout->addElement(ui_animCtrlButton, geoInfo2);

            ui_fpsLabel->f_onRendererUpdateObject2DAfter = [=](Panel* p, Renderer* rndr)
            {
                static UINT fps = 0;
                if (!wk_animCtrlButton.expired())
                {
                    if (wk_animCtrlButton.lock()->stateDetail().activated())
                    {
                        if (rndr->timer()->fps() != fps)
                        {
                            fps = rndr->timer()->fpsNum();
                            ((Label*)p)->setText(L"FPS: " + std::to_wstring(fps));
                        }
                    }
                }
            };
            ui_animCtrlButton->f_onStateChange = [=]
            (ToggleButton::StatefulObject* obj, ToggleButton::StatefulObject::Event& e)
            {
                if (!wk_fpsLabel.expired() && !wk_stickBoy.expired())
                {
                    auto sh_stickBoy = wk_stickBoy.lock();
                    auto sh_animCtrlButton = wk_animCtrlButton.lock();
                    if (e.activated())
                    {
                        sh_stickBoy->increaseAnimationCount();
                        sh_animCtrlButton->content()->label()->setText(L"Stop Anim");
                    }
                    else if (e.deactivated())
                    {
                        sh_stickBoy->decreaseAnimationCount();
                        sh_animCtrlButton->content()->label()->setText(L"Start Anim");
                        wk_fpsLabel.lock()->setText(L"FPS: None");
                    }
                }
            };
        }
        auto ui_timeSpanLabel = makeUIObject<Label>(L"Time Span");
        auto ui_timeSpanInput = makeUIObject<RawTextBox>(5.0f, math_utils::sizeOnlyRect({ 120.0f, 42.0f }));
        auto wk_timeSpanInput = (WeakPtr<RawTextBox>)ui_timeSpanInput;
        {
            ui_timeSpanLabel->setSize(250.0f, 50.0f);

            GridLayout::GeometryInfo geoInfo1 = {};
            geoInfo1.isFixedSize = true;
            geoInfo1.axis.x = { 0, 5 };
            geoInfo1.axis.y = { 2, 1 };
            ui_sideLayout->addElement(ui_timeSpanLabel, geoInfo1);

            ui_timeSpanInput->setText(L"0.06 s");
            ui_timeSpanInput->setVisibleTextRect({ 5.0f, 8.0f, 115.0f, 34.0f });

            GridLayout::GeometryInfo geoInfo2 = {};
            geoInfo2.isFixedSize = true;
            geoInfo2.axis.x = { 2, 3 };
            geoInfo2.axis.y = { 2, 1 };
            ui_sideLayout->addElement(ui_timeSpanInput, geoInfo2);

            ui_timeSpanInput->bringToFront();

            ui_timeSpanInput->f_onLoseKeyboardFocus = [=](Panel* p)
            {
                if (!wk_stickBoy.expired())
                {
                    auto pp = (RawTextBox*)p;
                    auto& fanim = wk_stickBoy.lock()->bitmapData.fanim;
                    try
                    {
                        std::wstringstream ss;
                        float timeSpanInSecs = std::stof(pp->text());
                        ss << std::fixed << std::setprecision(2) << timeSpanInSecs;

                        pp->setText(ss.str() + L" s");
                        fanim.timeSpanDataInSecs = timeSpanInSecs;
                    }
                    catch (...) // std::stof failed
                    {
                        pp->setText(L"0.06 s");
                        fanim.timeSpanDataInSecs = 0.06f;
                    }
                }
            };
        }
        ui_stickBoy->f_onDestroy = [=](Panel* p)
        {
            // No need to do the clearing if the application already destroyed.
            if (!Application::g_app) return;

            if (p->isPlayAnimation()) p->decreaseAnimationCount();

            if (!wk_fpsLabel.expired())
            {
                wk_fpsLabel.lock()->setText(L"FPS: None");
            }
            if (!wk_animCtrlButton.expired())
            {
                auto sh_animCtrlButton = wk_animCtrlButton.lock();
                sh_animCtrlButton->setEnabled(false);
                sh_animCtrlButton->setActivatedState(ToggleButton::Deactivated);
            }
            if (!wk_timeSpanInput.expired())
            {
                auto sh_timeSpanInput = wk_timeSpanInput.lock();
                sh_timeSpanInput->setEnabled(false);
                sh_timeSpanInput->LabelArea::setText(L"0.06 s");
            }
        };
        auto ui_frameSizeLabel = makeUIObject<Label>(L"Frame Size (in pixel)");
        auto ui_frameSizeSlider = makeUIObject<HorzSlider>();
        auto wk_frameSizeSlider = (WeakPtr<HorzSlider>)ui_frameSizeSlider;
        {
            ui_frameSizeLabel->setSize(250.0f, 50.0f);

            GridLayout::GeometryInfo geoInfo1 = {};
            geoInfo1.isFixedSize = true;
            geoInfo1.axis.x = { 0, 5 };
            geoInfo1.axis.y = { 3, 1 };
            ui_sideLayout->addElement(ui_frameSizeLabel, geoInfo1);

            ui_frameSizeSlider->setSize(250.0f, 50.0f);

            ui_frameSizeSlider->setMinValue(64.0f);
            ui_frameSizeSlider->setMaxValue(512.0f);
            ui_frameSizeSlider->setValue(256.0f);

            ui_frameSizeSlider->stepMode = Slider::StepMode::Discrete;
            ui_frameSizeSlider->stepInterval = 16.0f;

            auto& valueLabelAppear = ui_frameSizeSlider->appearance().valueLabel;
            valueLabelAppear.resident = true;

            GridLayout::GeometryInfo geoInfo2 = {};
            geoInfo2.isFixedSize = true;
            geoInfo2.axis.x = { 0, 5 };
            geoInfo2.axis.y = { 4, 1 };
            ui_sideLayout->addElement(ui_frameSizeSlider, geoInfo2);

            ui_frameSizeSlider->f_onValueChange = [=]
            (Slider::ValuefulObject* vobj, float value)
            {
                if (!wk_pixelViewer.expired())
                {
                    auto sh_pixelViewer = wk_pixelViewer.lock();
                    auto& index = sh_pixelViewer->selectedTabIndex();
                    if (index.has_value())
                    {
                        auto& item = sh_pixelViewer->tabs()[index.value()];
                        auto layout = std::dynamic_pointer_cast<GridLayout>(item.content);
                        if (layout)
                        {
                            auto itor = layout->children().begin();
                            if (itor != layout->children().end())
                            {
                                (*itor)->setSize(value, value);
                                layout->updateAllElements();
                            }
                        }
                    }
                }
            };
            ui_pixelViewer->f_onSelectedTabChange = [=]
            (TabGroup * tg, OptRefer<size_t> index)
            {
                if (index.has_value() && !wk_frameSizeSlider.expired())
                {
                    auto& item = tg->tabs()[index.value()];
                    auto layout = std::dynamic_pointer_cast<GridLayout>(item.content);
                    if (layout)
                    {
                        auto itor = layout->children().begin();
                        if (itor != layout->children().end())
                        {
                            wk_frameSizeSlider.lock()->setValue((*itor)->width());
                        }
                    }
                }
            };
        }
    });
}
