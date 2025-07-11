﻿#include "Common/Precompile.h"

#include "Common/DirectXError.h"
#include "Common/MathUtils/Basic.h"

#include "UIKit/AppEntry.h"
#include "UIKit/Application.h"
#include "UIKit/BitmapUtils.h"
#include "UIKit/ComboBox.h"
#include "UIKit/FileSystemUtils.h"
#include "UIKit/GridLayout.h"
#include "UIKit/HorzSlider.h"
#include "UIKit/IconLabel.h"
#include "UIKit/LabelArea.h"
#include "UIKit/MainWindow.h"
#include "UIKit/OnOffSwitch.h"
#include "UIKit/OutlinedButton.h"
#include "UIKit/PopupMenu.h"
#include "UIKit/TabCaption.h"
#include "UIKit/TabGroup.h"

using namespace d14engine;
using namespace d14engine::uikit;

#define D14_DEMO_NAME L"VariousFonts"

#define D14_MAINWINDOW_TITLE L"D14Engine - " D14_DEMO_NAME L" @ UIKit"
#define D14_SCREENSHOT_PATH L"Screenshots/" D14_DEMO_NAME L".png"

D14_SET_APP_ENTRY(mainVariousFonts)
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
        auto ui_textViewer = makeManagedUIObject<TabGroup>(ui_clientArea);
        auto wk_textViewer = (WeakPtr<TabGroup>)ui_textViewer;
        {
            ui_textViewer->transform(0.0f, 50.0f, 800.0f, 300.0f);

#define SET_TAB_SIZE(State, Width, Height) \
    ui_textViewer->appearance().tabBar.tab.main \
    [(size_t)TabGroup::TabState::State].geometry.size = { Width, Height }

            SET_TAB_SIZE(Idle, 250.0f, 32.0f);
            SET_TAB_SIZE(Hovered, 250.0f, 32.0f);
            SET_TAB_SIZE(Selected, 266.0f, 40.0f);

#undef SET_TAB_SIZE
            ui_textViewer->selectedTabRes.loadMask();
            ui_textViewer->selectedTabRes.loadPathGeo();

            auto& barAppear = ui_textViewer->appearance().tabBar;

            barAppear.geometry.height = 40.0f;
            barAppear.separator.geometry.size.height = 24.0f;
            barAppear.overflow.button.geometry.offset.y = 7.0f;
        }
        Wstring excerptDir = L"Test/UIKit/VariousFonts/";

        auto getFileSize = [](WstrRefer filePath) -> size_t
        {
            struct _stat fileinfo;
            _wstat(filePath.c_str(), &fileinfo);
            return fileinfo.st_size;
        };
        file_system_utils::foreachFileInDir(excerptDir, L"*.txt", [&](WstrRefer filePath)
        {
            auto fileName = file_system_utils::extractFileName(filePath);
            auto filePrefix = file_system_utils::extractFilePrefix(fileName);

            auto ui_caption = makeUIObject<TabCaption>(filePrefix);

            ui_caption->appearance().title.rightPadding = 12.0f;

            ui_caption->closable = false;
            ui_caption->title()->label()->setTextFormat(D14_FONT(L"Default/14"));

#pragma warning(push)
#pragma warning(disable : 4996)
            FILE* fileHandle = _wfopen(filePath.c_str(), L"r, ccs=UNICODE");
#pragma warning(pop)
            THROW_IF_NULL(fileHandle);

            Wstring fileBuffer = {};
            auto fileSize = getFileSize(filePath);
            if (fileSize > 0)
            {
                fileBuffer.resize(fileSize);
                fread(fileBuffer.data(), sizeof(WCHAR), fileSize, fileHandle);
            }
            auto ui_block = makeUIObject<LabelArea>(fileBuffer, math_utils::widthOnlyRect(750.0f));
            auto wk_block = (WeakPtr<LabelArea>)ui_block;

            ui_block->setTextFormat(D14_FONT(L"Default/16"));
            THROW_IF_FAILED(ui_block->textLayout()->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP));

            auto ui_wrapper = makeUIObject<ConstraintLayout>();
            ui_wrapper->setSize(800.0f, ui_block->textMetrics().height + 100.0f);

            ConstraintLayout::GeometryInfo geoInfo = {};

            geoInfo.keepWidth = false;
            geoInfo.Left.ToLeft = 25.0f;
            geoInfo.Right.ToRight = 25.0f;

            geoInfo.keepHeight = false;
            geoInfo.Top.ToTop = 50.0f;
            geoInfo.Bottom.ToBottom = 50.0f;

            ui_wrapper->addElement(ui_block, geoInfo);

            auto ui_content = makeUIObject<ScrollView>(ui_wrapper);

            // Set opaque background to support rendering ClearType text.
            ui_content->appearance().background.opacity = 1.0f;

            // Keep the hilite range when scrolling view with mouse button.
            //ui_content->f_onStartThumbScrolling = [=]
            //(ScrollView* sv, const D2D1_POINT_2F& offset)
            //{
            //    if (!wk_block.expired())
            //    {
            //        wk_block.lock()->keepHiliteRange = true;
            //    }
            //};
            //ui_content->f_onEndThumbScrolling = [=]
            //(ScrollView* sv, const D2D1_POINT_2F& offset)
            //{
            //    if (!wk_block.expired())
            //    {
            //        auto sh_block = wk_block.lock();
            //        app->focusUIObject(sh_block);
            //        sh_block->keepHiliteRange = false;
            //    }
            //};
            ui_textViewer->appendTab({{ ui_caption, ui_content }});
            return false;
        });
        ui_textViewer->setSelectedTab(0);

        // Load dependent text formats.

        std::vector<Wstring> fontNameArray =
        {
            L"Segoe UI", L"Arial", L"Times New Roman",
            L"微软雅黑", L"楷体", L"宋体"
        };
        std::vector<Wstring> fontLocaleNameArray =
        {
            L"en-us", L"en-us", L"en-us", L"zh-cn", L"zh-cn", L"zh-cn"
        };
        std::vector<Wstring> fontWeightArray =
        {
            L"Light", L"Semi-Light", L"Normal", L"Semi-Bold", L"Bold"
        };
        std::vector<DWRITE_FONT_WEIGHT> fontWeightEnumArray =
        {
            DWRITE_FONT_WEIGHT_LIGHT, DWRITE_FONT_WEIGHT_SEMI_LIGHT, DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_WEIGHT_BOLD
        };
        auto textFormatMap = std::make_shared<resource_utils::TextFormatMap>();
        for (size_t n = 0; n < fontNameArray.size(); ++n)
        {
            for (size_t w = 0; w < fontWeightArray.size(); ++w)
            {
                for (int fontSize = 11; fontSize <= 32; ++fontSize)
                {
                    // Method 1: Use the built-in resource_utils.

                    //resource_utils::loadSystemTextFormat
                    //(
                    //    /* textFormatName */ textFormatName,
                    //    /* fontFamilyName */ fontNameArray[n],
                    //    /* fontSize       */ (float)fontSize,
                    //    /* localeName     */ fontLocaleNameArray[n],
                    //    /* fontWeight     */ fontWeightEnumArray[w]
                    //);

                    // Method 2: Create and manage it yourself.

                    auto textFormatName =
                        fontNameArray[n] + L"/" +
                        fontWeightArray[w] + L"/" +
                        std::to_wstring(fontSize);

                    auto factory = app->renderer()->dwriteFactory();
                    THROW_IF_FAILED(factory->CreateTextFormat(
                        fontNameArray[n].c_str(),
                        nullptr,
                        fontWeightEnumArray[w],
                        DWRITE_FONT_STYLE_NORMAL,
                        DWRITE_FONT_STRETCH_NORMAL,
                        // 1 inch == 72 pt == 96 dip
                        fontSize * 96.0f / 72.0f,
                        fontLocaleNameArray[n].c_str(),
                        &(*textFormatMap)[textFormatName]));
                }
            }
        }
        // Create bottom control-panel.

        auto ui_controlPanel = makeManagedUIObject<GridLayout>(ui_clientArea);
        {
            ui_controlPanel->transform(0.0f, 374.0f, 800.0f, 190.0f);
            ui_controlPanel->appearance().background.opacity = 1.0f;
            ui_controlPanel->setCellCount(10, 2);
        }
        auto ui_fontNameSelector = makeUIObject<ComboBox>(5.0f);
        auto wk_fontNameSelector = (WeakPtr<ComboBox>)ui_fontNameSelector;
        {
            ui_fontNameSelector->setSize(240.0f, 40.0f);

            GridLayout::GeometryInfo geoInfo1 = {};
            geoInfo1.isFixedSize = true;
            geoInfo1.axis.x = { 1, 4 };
            geoInfo1.axis.y = { 0, 1 };
            ui_controlPanel->addElement(ui_fontNameSelector, geoInfo1);

            auto ui_label = makeUIObject<Label>(L"Text font");

            GridLayout::GeometryInfo geoInfo2 = {};
            geoInfo2.isFixedSize = false;
            geoInfo2.axis.x = { 0, 1 };
            geoInfo2.axis.y = { 0, 1 };
            geoInfo2.spacing.left = 24.0f;
            ui_controlPanel->addElement(ui_label, geoInfo2);

            PopupMenu::ItemArray fontNameItems = {};
            for (auto& fontName : fontNameArray)
            {
                auto content = IconLabel::comboBoxLayout(fontName);

                content->label()->setTextFormat(
                    textFormatMap->at(fontName + L"/Normal/16").Get());

                fontNameItems.push_back(makeUIObject<MenuItem>(
                    content, math_utils::heightOnlyRect(40.0f)));
            }
            auto& dropDownMenu = ui_fontNameSelector->dropDownMenu();

            dropDownMenu->setSize(dropDownMenu->width(), 240.0f);
            dropDownMenu->appendItem(fontNameItems);

            ui_fontNameSelector->menuOffset = { 0.0f, -240.0f };
            ui_fontNameSelector->setSelected(0);
        }
        auto ui_textAntialiasModeSelector = makeUIObject<ComboBox>(5.0f);
        {
            ui_textAntialiasModeSelector->setSize(160.0f, 40.0f);

            GridLayout::GeometryInfo geoInfo1 = {};
            geoInfo1.isFixedSize = true;
            geoInfo1.axis.x = { 7, 3 };
            geoInfo1.axis.y = { 0, 1 };
            ui_controlPanel->addElement(ui_textAntialiasModeSelector, geoInfo1);

            auto ui_label = makeUIObject<Label>(L"Text antialias mode");

            GridLayout::GeometryInfo geoInfo2 = {};
            geoInfo2.isFixedSize = false;
            geoInfo2.axis.x = { 5, 2 };
            geoInfo2.axis.y = { 0, 1 };
            ui_controlPanel->addElement(ui_label, geoInfo2);

            std::vector<std::pair<Wstring, D2D1_TEXT_ANTIALIAS_MODE>> textAntialiasModePairs =
            {
                { L"Default", D2D1_TEXT_ANTIALIAS_MODE_DEFAULT },
                { L"ClearType", D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE },
                { L"Grayscale", D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE }
            };
            PopupMenu::ItemArray modeItems = {};
            for (auto& mode : textAntialiasModePairs)
            {
                modeItems.push_back(makeUIObject<MenuItem>(
                    IconLabel::comboBoxLayout(mode.first), math_utils::heightOnlyRect(40.0f)));
            }
            auto& dropDownMenu = ui_textAntialiasModeSelector->dropDownMenu();

            dropDownMenu->setSize(dropDownMenu->width(), 120.0f);
            dropDownMenu->appendItem(modeItems);

            ui_textAntialiasModeSelector->setSelected(0);

            using TextAntialiasModeMap = std::unordered_map<Wstring, D2D1_TEXT_ANTIALIAS_MODE>;
            ui_textAntialiasModeSelector->f_onSelectedChange =
            [=,
                textAntialiasModeMap = TextAntialiasModeMap
                {
                    textAntialiasModePairs.begin(), textAntialiasModePairs.end()
                }
            ]
            (ComboBox* cb, OptRefer<size_t> index)
            {
                auto& modeStr = cb->content()->label()->text();
                app->renderer()->setTextAntialiasMode(textAntialiasModeMap.at(modeStr));
            };
        }
        auto ui_fontSizeSlider = makeUIObject<HorzSlider>();
        auto wk_fontSizeSlider = (WeakPtr<HorzSlider>)ui_fontSizeSlider;
        {
            ui_fontSizeSlider->setSize(210.0f, 40.0f);

            ui_fontSizeSlider->setMinValue(11.0f);
            ui_fontSizeSlider->setMaxValue(32.0f);
            ui_fontSizeSlider->setValue(16.0f);

            ui_fontSizeSlider->stepMode = Slider::StepMode::Discrete;
            ui_fontSizeSlider->stepInterval = 1.0f;

            auto& valueLabelAppear = ui_fontSizeSlider->appearance().valueLabel;
            valueLabelAppear.offset = 14.0f;
            valueLabelAppear.mainRect.geometry.size = { 100.0f, 40.0f };
            ui_fontSizeSlider->valueLabelRes.loadShadowMask();
            ui_fontSizeSlider->sideTriangleRes.loadPathGeo();
            valueLabelAppear.resident = true;

            ui_fontSizeSlider->valueLabel()->setTextFormat(D14_FONT(L"Default/16"));

            GridLayout::GeometryInfo geoInfo1 = {};
            geoInfo1.isFixedSize = true;
            geoInfo1.axis.x = { 1, 4 };
            geoInfo1.axis.y = { 1, 1 };
            ui_controlPanel->addElement(ui_fontSizeSlider, geoInfo1);

            auto ui_label = makeUIObject<Label>(L"Font size");

            GridLayout::GeometryInfo geoInfo2 = {};
            geoInfo2.isFixedSize = false;
            geoInfo2.axis.x = { 0, 1 };
            geoInfo2.axis.y = { 1, 1 };
            geoInfo2.spacing.left = 24.0f;
            ui_controlPanel->addElement(ui_label, geoInfo2);
        }
        auto ui_fontWeightSelector = makeUIObject<HorzSlider>();
        auto wk_fontWeightSelector = (WeakPtr<HorzSlider>)ui_fontWeightSelector;
        {
            ui_fontWeightSelector->setSize(180.0f, 40.0f);

            ui_fontWeightSelector->setMinValue(0.0f);
            ui_fontWeightSelector->setMaxValue(4.0f);
            ui_fontWeightSelector->setValue(2.0f);

            ui_fontWeightSelector->stepMode = Slider::StepMode::Discrete;
            ui_fontWeightSelector->stepInterval = 1.0f;

            auto& valueLabelAppear = ui_fontWeightSelector->appearance().valueLabel;
            valueLabelAppear.offset = 14.0f;
            valueLabelAppear.mainRect.geometry.size = { 120.0f, 40.0f };
            ui_fontWeightSelector->valueLabelRes.loadShadowMask();
            ui_fontWeightSelector->sideTriangleRes.loadPathGeo();
            valueLabelAppear.resident = true;

            ui_fontWeightSelector->valueLabel()->setTextFormat(D14_FONT(L"Default/16"));

            GridLayout::GeometryInfo geoInfo1 = {};
            geoInfo1.isFixedSize = true;
            geoInfo1.axis.x = { 6, 4 };
            geoInfo1.axis.y = { 1, 1 };
            ui_controlPanel->addElement(ui_fontWeightSelector, geoInfo1);

            auto ui_label = makeUIObject<Label>(L"Font weight");

            GridLayout::GeometryInfo geoInfo2 = {};
            geoInfo2.isFixedSize = false;
            geoInfo2.axis.x = { 5, 1 };
            geoInfo2.axis.y = { 1, 1 };
            ui_controlPanel->addElement(ui_label, geoInfo2);
        }

        // Set control-panel callbacks.

        auto changeTextBlockFont = [=](IDWriteTextFormat* textFormat)
        {
            if (!wk_textViewer.expired())
            {
                auto sh_textViewer = wk_textViewer.lock();
                auto& index = sh_textViewer->selectedTabIndex();
                if (index.has_value())
                {
                    auto& tab = sh_textViewer->tabs()[index.value()];

                    if (tab.content->children().empty()) return;
                    auto layout = tab.content->children().begin();

                    if ((*layout)->children().empty()) return;
                    auto element = (*layout)->children().begin();

                    auto tblock = std::dynamic_pointer_cast<Label>(*element);
                    if (tblock != nullptr)
                    {
                        tblock->setTextFormat(textFormat);
                    }
                    (*layout)->setSize(800.0f, tblock->textMetrics().height + 100.0f);

                    // Update the viewport offset of the scroll view to fix display bug.
                    auto content = std::dynamic_pointer_cast<ScrollView>(tab.content);
                    if (content != nullptr)
                    {
                        content->setViewportOffset(content->viewportOffset());
                    }
                }
            }
        };
        ui_fontNameSelector->f_onSelectedChange = [=](ComboBox* cb, OptRefer<size_t> index)
        {
            if (!wk_fontSizeSlider.expired() && !wk_fontWeightSelector.expired())
            {
                auto sh_fontSizeSlider = wk_fontSizeSlider.lock();
                auto sh_fontWeightSlider = wk_fontWeightSelector.lock();

                auto textFormatName =
                    cb->content()->label()->text() + L"/" +
                    sh_fontWeightSlider->valueLabel()->text() + L"/" +
                    std::to_wstring(math_utils::round(sh_fontSizeSlider->value()));

                changeTextBlockFont(textFormatMap->at(textFormatName).Get());
            }
        };
        ui_fontSizeSlider->f_onValueChange = [=](Slider::ValuefulObject* vobj, float value)
        {
            auto sldr = (Slider*)vobj;
            sldr->valueLabel()->setText(sldr->valueLabel()->text() + L" pt");

            if (!wk_fontNameSelector.expired() && !wk_fontWeightSelector.expired())
            {
                auto sh_fontNameSelector = wk_fontNameSelector.lock();
                auto sh_fontWeightSlider = wk_fontWeightSelector.lock();

                auto textFormatName =
                    sh_fontNameSelector->content()->label()->text() + L"/" +
                    sh_fontWeightSlider->valueLabel()->text() + L"/" +
                    std::to_wstring(math_utils::round(value));

                changeTextBlockFont(textFormatMap->at(textFormatName).Get());
            }
        };
        ui_fontWeightSelector->f_onValueChange = [=](Slider::ValuefulObject* vobj, float value)
        {
            auto sldr = (Slider*)vobj;
            sldr->valueLabel()->setText(fontWeightArray[math_utils::round(value)]);

            if (!wk_fontNameSelector.expired() && !wk_fontSizeSlider.expired())
            {
                auto sh_fontNameSelector = wk_fontNameSelector.lock();
                auto sh_fontSizeSlider = wk_fontSizeSlider.lock();

                auto textFormatName =
                    sh_fontNameSelector->content()->label()->text() + L"/" +
                    sldr->valueLabel()->text() + L"/" +
                    std::to_wstring(math_utils::round(sh_fontSizeSlider->value()));

                changeTextBlockFont(textFormatMap->at(textFormatName).Get());
            }
        };
        // We must update font-weight firstly since the callback of font-size
        // depends on the content of the value-label of the font-weight-panel.
        ui_fontWeightSelector->onValueChange(ui_fontWeightSelector->value());
        ui_fontSizeSlider->onValueChange(ui_fontSizeSlider->value());
    });
}
