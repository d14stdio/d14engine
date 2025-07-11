﻿#include "Common/Precompile.h"

#include "Settings.h"

#include "Common/DirectXError.h"

#include "Renderer/TickTimer.h"

#include "UIKit/Application.h"
#include "UIKit/BitmapUtils.h"
#include "UIKit/CheckBox.h"
#include "UIKit/ComboBox.h"
#include "UIKit/Cursor.h"
#include "UIKit/HorzSlider.h"
#include "UIKit/IconLabel.h"
#include "UIKit/MenuItem.h"
#include "UIKit/OnOffSwitch.h"
#include "UIKit/PopupMenu.h"
#include "UIKit/RawTextBox.h"
#include "UIKit/ScrollView.h"

using namespace d14engine;
using namespace d14engine::renderer;
using namespace d14engine::uikit;

void createSettingsTabPage(ConstraintLayout* page)
{
    auto ui_sideLayout = makeUIObject<ConstraintLayout>();
    {
        ConstraintLayout::GeometryInfo geoInfo = {};

        geoInfo.keepWidth = false;
        geoInfo.Left.ToLeft = 0.0f;
        geoInfo.Right.ToLeft = 350.0f;

        geoInfo.keepHeight = false;
        geoInfo.Top.ToTop = 24.0f;
        geoInfo.Bottom.ToBottom = 0.0f;

        page->addElement(ui_sideLayout, geoInfo);
    }
    auto ui_aboutTitle = makeUIObject<Label>(L"About");
    {
        ui_aboutTitle->setTextFormat(D14_FONT(L"Default/22"));
        ui_aboutTitle->hardAlignment.vert = Label::VertAlignment::Top;

        ConstraintLayout::GeometryInfo geoInfo = {};
        geoInfo.Left.ToLeft = 40.0f;
        geoInfo.Top.ToTop = 60.0f;
        ui_sideLayout->addElement(ui_aboutTitle, geoInfo);
    }
    auto ui_aboutLabel = makeUIObject<Label>(L"UIKit @ D14Engine");
    {
        ui_aboutTitle->hardAlignment.vert = Label::VertAlignment::Top;

        ConstraintLayout::GeometryInfo geoInfo = {};
        geoInfo.Left.ToLeft = 40.0f;
        geoInfo.Top.ToTop = 120.0f;
        ui_sideLayout->addElement(ui_aboutLabel, geoInfo);
    }
    auto ui_aboutButton = makeUIObject<FlatButton>(L"D14Engine repo @ GitHub", 5.0f);
    {
        ui_aboutButton->setSize(250.0f, 40.0f);

        auto& ui_btnLabel = ui_aboutButton->content()->label();
        ui_btnLabel->setTextFormat(D14_FONT(L"Default/14"));
        THROW_IF_FAILED(ui_btnLabel->textLayout()->SetUnderline(true, { 0, UINT32_MAX }));

        ui_aboutButton->f_onChangeThemeStyle = [=](Panel* p, const Panel::ThemeStyle& style)
        {
            auto& appear = dynamic_cast<FlatButton*>(p)->appearance();

#define BUTTON_FOREGROUND_COLOR(State_Name) \
    appear.main[(size_t)ButtonState::State_Name].foreground.color

            BUTTON_FOREGROUND_COLOR(Idle) = BUTTON_FOREGROUND_COLOR(Hover) =
            BUTTON_FOREGROUND_COLOR(Down) = appearance::color1();

#undef BUTTON_FOREGROUND_COLOR
        };
        ui_aboutButton->f_onMouseMove = [](Panel* p, MouseMoveEvent& e)
        {
            Application::g_app->cursor()->setIcon(Cursor::Hand);
        };
        ui_aboutButton->f_onMouseButtonRelease = [](ClickablePanel* clkp, ClickablePanel::Event& e)
        {
            if (e.left())
            {
                auto wnd = Application::g_app->win32Window();
                auto url = L"https://github.com/DreamersGather/D14Engine";
                ShellExecute(wnd, nullptr, url, nullptr, nullptr, SW_NORMAL);
            }
        };
        ConstraintLayout::GeometryInfo geoInfo = {};
        geoInfo.Left.ToLeft = 25.0f;
        geoInfo.Top.ToTop = 150.0f;
        ui_sideLayout->addElement(ui_aboutButton, geoInfo);
    }
    auto ui_engineIcon = makeUIObject<Panel>(math_utils::sizeOnlyRect({ 266.0f, 55.0f }));
    {
        ui_engineIcon->bitmap = bitmap_utils::loadBitmap(
            L"Test/UIKit/WidgetsGallery/Images/AboutPanel/D14Engine.png");

        ConstraintLayout::GeometryInfo geoInfo = {};
        geoInfo.Left.ToLeft = 40.0f;
        geoInfo.Top.ToTop = 220.0f;
        ui_sideLayout->addElement(ui_engineIcon, geoInfo);
    }
    auto ui_uikitIcon = makeUIObject<Panel>(math_utils::sizeOnlyRect({ 233.0f, 55.0f }));
    {
        ui_uikitIcon->bitmap = bitmap_utils::loadBitmap(
            L"Test/UIKit/WidgetsGallery/Images/AboutPanel/D14UIKit.png");

        ConstraintLayout::GeometryInfo geoInfo = {};
        geoInfo.Left.ToLeft = 40.0f;
        geoInfo.Top.ToTop = 300.0f;
        ui_sideLayout->addElement(ui_uikitIcon, geoInfo);
    }
    auto ui_aboutComment = makeUIObject<Label>(L"—— D14 means DreamersGather.");
    {
        ui_aboutComment->setTextFormat(D14_FONT(L"Default/14"));
        THROW_IF_FAILED(ui_aboutComment->textLayout()->SetFontStyle(DWRITE_FONT_STYLE_ITALIC, { 0, UINT32_MAX }));
        ui_aboutComment->hardAlignment.vert = Label::VertAlignment::Top;

        ConstraintLayout::GeometryInfo geoInfo = {};
        geoInfo.Left.ToLeft = 40.0f;
        geoInfo.Top.ToTop = 400.0f;
        ui_sideLayout->addElement(ui_aboutComment, geoInfo);
    }
    auto ui_contentLayout = makeUIObject<ConstraintLayout>();
    {
        auto& appear = ui_contentLayout->appearance();

        appear.background.opacity = 1.0f;
        appear.stroke.width = 2.0f;
        appear.stroke.opacity = 1.0f;

        auto& light = appear.g_themeData.at(L"Light");
        {
            light.background.color = D2D1::ColorF{ 0xf9f9f9 };
        }
        auto& dark = appear.g_themeData.at(L"Dark");
        {
            dark.background.color = D2D1::ColorF{ 0x272727 };
        }
        ui_contentLayout->roundRadiusX = ui_contentLayout->roundRadiusY = 10.0f;

        ConstraintLayout::GeometryInfo geoInfo = {};

        geoInfo.keepWidth = false;
        geoInfo.Left.ToLeft = 350.0f;
        geoInfo.Right.ToRight = -10.0f;

        geoInfo.keepHeight = false;
        geoInfo.Top.ToTop = 24.0f;
        geoInfo.Bottom.ToBottom = -10.0f;

        page->addElement(ui_contentLayout, geoInfo);
    }
    auto ui_topTitle = makeUIObject<Label>(L"Settings");
    {
        ui_topTitle->setTextFormat(D14_FONT(L"Default/22"));
        ui_topTitle->hardAlignment.vert = Label::VertAlignment::Top;

        ConstraintLayout::GeometryInfo geoInfo = {};
        geoInfo.Left.ToLeft = 70.0f;
        geoInfo.Top.ToTop = 60.0f;
        ui_contentLayout->addElement(ui_topTitle, geoInfo);
    }
    auto ui_bottomView = makeUIObject<ScrollView>();
    {
        ui_bottomView->deltaDipsPerScroll.vert = 90.0f;

        ConstraintLayout::GeometryInfo geoInfo = {};

        geoInfo.keepWidth = false;
        geoInfo.Left.ToLeft = 0.0f;
        geoInfo.Right.ToRight = 20.0f;

        geoInfo.keepHeight = false;
        geoInfo.Top.ToTop = 100.0f;
        geoInfo.Bottom.ToBottom = 30.0f;

        ui_contentLayout->addElement(ui_bottomView, geoInfo);

        ui_bottomView->f_onSize = [](Panel* p, SizeEvent& e)
        {
            auto content = dynamic_cast<ScrollView*>(p)->content();
            if (!content.expired())
            {
                auto pContent = content.lock();
                pContent->setSize(e.size.width, pContent->height());
            }
        };
    }
    auto ui_settingsLayout = makeUIObject<ConstraintLayout>();
    {
        ui_settingsLayout->setSize(920.0f, 1000.0f);

        auto& appear = ui_settingsLayout->appearance();
        appear.background.opacity = 1.0f;

        auto& light = appear.g_themeData.at(L"Light");
        {
            light.background.color = D2D1::ColorF{ 0xf9f9f9 };
        }
        auto& dark = appear.g_themeData.at(L"Dark");
        {
            dark.background.color = D2D1::ColorF{ 0x272727 };
        }
        ui_bottomView->setContent(ui_settingsLayout);
    }
    auto ui_themeModeLabel = makeUIObject<Label>(L"Theme Mode");
    {
        ui_themeModeLabel->setTextFormat(D14_FONT(L"Default/18"));
        ui_themeModeLabel->hardAlignment.vert = Label::VertAlignment::Top;

        ConstraintLayout::GeometryInfo geoInfo = {};
        geoInfo.Left.ToLeft = 70.0f;
        geoInfo.Top.ToTop = 20.0f;
        ui_settingsLayout->addElement(ui_themeModeLabel, geoInfo);
    }
    auto ui_autoThemeCheckBox = makeUIObject<CheckBox>();
    auto wk_autoThemeCheckBox = (WeakPtr<CheckBox>)ui_autoThemeCheckBox;
    {
        ConstraintLayout::GeometryInfo geoInfo1 = {};
        geoInfo1.Left.ToLeft = 70.0f;
        geoInfo1.Top.ToTop = 63.0f;
        ui_settingsLayout->addElement(ui_autoThemeCheckBox, geoInfo1);

        auto ui_label = makeManagedUIObject<Label>(ui_autoThemeCheckBox, L"Use system setting");
        auto wk_label = (WeakPtr<Label>)ui_label;

        ui_autoThemeCheckBox->reorderAbovePeerObject(ui_label.get());

        ui_label->transform(34.0f, 0.0f, 0.0f, ui_autoThemeCheckBox->height());
        ui_label->setTextFormat(D14_FONT(L"Default/14"));
        ui_label->hardAlignment.vert = Label::VertAlignment::Center;

        ui_autoThemeCheckBox->f_isHit = [=](const Panel* p, const Event::Point& pt)
        {
            float horzOffset = 0.0f;
            if (!wk_label.expired()) horzOffset = 10.0f + wk_label.lock()->textAreaSize().width;
            return math_utils::isOverlapped(pt, math_utils::increaseRight(p->absoluteRect(), horzOffset));
        };
    }
    auto ui_darkModeSwitch = makeUIObject<OnOffSwitch>();
    auto wk_darkModeSwitch = (WeakPtr<OnOffSwitch>)ui_darkModeSwitch;
    {
        ConstraintLayout::GeometryInfo geoInfo1 = {};
        geoInfo1.Left.ToLeft = 70.0f;
        geoInfo1.Top.ToTop = 107.0f;
        ui_settingsLayout->addElement(ui_darkModeSwitch, geoInfo1);

        auto ui_label = makeManagedUIObject<Label>(ui_darkModeSwitch, L"Dark mode");
        auto wk_label = (WeakPtr<Label>)ui_label;

        ui_darkModeSwitch->reorderAbovePeerObject(ui_label.get());

        ui_label->transform(63.0f, 0.0f, 0.0f, ui_darkModeSwitch->height());
        ui_label->setTextFormat(D14_FONT(L"Default/14"));
        ui_label->hardAlignment.vert = Label::VertAlignment::Center;

        ui_darkModeSwitch->f_isHit = [=](const Panel* p, const Event::Point& pt)
        {
            float horzOffset = 0.0f;
            if (!wk_label.expired()) horzOffset = 15.0f + wk_label.lock()->textAreaSize().width;
            return math_utils::isOverlapped(pt, math_utils::increaseRight(p->absoluteRect(), horzOffset));
        };

        Application::g_app->f_onSystemThemeStyleChange = [=](const Application::ThemeStyle& style)
        {
            if (!wk_autoThemeCheckBox.expired() && wk_autoThemeCheckBox.lock()->stateDetail().checked())
            {
                if (!wk_darkModeSwitch.expired())
                {
                    auto sh_darkModeSwitch = wk_darkModeSwitch.lock();
                    bool light = (style.name == L"Light");
                    sh_darkModeSwitch->setOnOff(light ? OnOffSwitch::Off : OnOffSwitch::On);
                }
            }
        };
        ui_autoThemeCheckBox->f_onStateChange = [=]
        (CheckBox::StatefulObject* obj, CheckBox::StatefulObject::Event& e)
        {
            if (e.unchecked()) // user selected
            {
                if (!wk_darkModeSwitch.expired()) wk_darkModeSwitch.lock()->setEnabled(true);
                if (!wk_label.expired()) wk_label.lock()->setEnabled(true);
            }
            if (e.checked()) // system setting
            {
                if (!wk_darkModeSwitch.expired())
                {
                    auto sh_darkModeSwitch = wk_darkModeSwitch.lock();
                    sh_darkModeSwitch->setEnabled(false);
                    auto style = Application::querySystemThemeStyle();
                    bool light = (style.name == L"Light");
                    sh_darkModeSwitch->setOnOff(light ? OnOffSwitch::Off : OnOffSwitch::On);
                }
                if (!wk_label.expired()) wk_label.lock()->setEnabled(false);
            }
        };
        ui_darkModeSwitch->f_onStateChange = []
        (OnOffSwitch::StatefulObject* obj, OnOffSwitch::StatefulObject::Event& e)
        {
            Application::ThemeStyle style = Application::g_app->themeStyle();
            if (e.on()) style.name = L"Dark";
            else if (e.off()) style.name = L"Light";
            Application::g_app->setThemeStyle(style);
        };
        ui_autoThemeCheckBox->setCheckState(CheckBox::Checked);
    }
    auto ui_antialiasModeLabel = makeUIObject<Label>(L"Antialias Mode");
    {
        ui_antialiasModeLabel->setTextFormat(D14_FONT(L"Default/18"));
        ui_antialiasModeLabel->hardAlignment.vert = Label::VertAlignment::Top;

        ConstraintLayout::GeometryInfo geoInfo = {};
        geoInfo.Left.ToLeft = 70.0f;
        geoInfo.Top.ToTop = 191.0f;
        ui_settingsLayout->addElement(ui_antialiasModeLabel, geoInfo);
    }
    auto ui_2dAntialiasModeSelector = makeUIObject<ComboBox>(5.0f);
    {
        ui_2dAntialiasModeSelector->setSize(200.0f, 40.0f);

        ConstraintLayout::GeometryInfo geoInfo1 = {};
        geoInfo1.Left.ToLeft = 130.0f;
        geoInfo1.Top.ToTop = 234.0f;
        ui_settingsLayout->addElement(ui_2dAntialiasModeSelector, geoInfo1);

        std::vector<std::pair<Wstring, D2D1_ANTIALIAS_MODE>> strModeArray =
        {
            { L"Aliased", D2D1_ANTIALIAS_MODE_ALIASED },
            { L"Per Primitive", D2D1_ANTIALIAS_MODE_PER_PRIMITIVE }
        };
        PopupMenu::ItemArray strModeItems = {};
        for (auto& strMode : strModeArray)
        {
            auto ui_content = IconLabel::comboBoxLayout(strMode.first);
            ui_content->label()->setTextFormat(D14_FONT(L"Default/14"));

            strModeItems.push_back(makeUIObject<MenuItem>(ui_content, math_utils::heightOnlyRect(40.0f)));
        }
        auto& dropDownMenu = ui_2dAntialiasModeSelector->dropDownMenu();

        dropDownMenu->setSize(dropDownMenu->width(), 80.0f);
        dropDownMenu->appendItem(strModeItems);

        using StrModeMap = std::unordered_map<Wstring, D2D1_ANTIALIAS_MODE>;
        ui_2dAntialiasModeSelector->f_onSelectedChange =
        [=,
            strModeMap = StrModeMap{ strModeArray.begin(), strModeArray.end() }
        ]
        (ComboBox* cb, OptRefer<size_t> index)
        {
            auto& text = cb->content()->label()->text();
            Application::g_app->renderer()->setAntialiasMode2D(strModeMap.at(text));
        };
        ui_2dAntialiasModeSelector->setSelected(1);

        auto ui_label = makeUIObject<Label>(L"2D", math_utils::heightOnlyRect(40.0f));

        ui_label->setTextFormat(D14_FONT(L"Default/14"));
        ui_label->hardAlignment.vert = Label::VertAlignment::Center;

        ConstraintLayout::GeometryInfo geoInfo2 = {};
        geoInfo2.Left.ToLeft = 70.0f;
        geoInfo2.Top.ToTop = 234.0f;
        ui_settingsLayout->addElement(ui_label, geoInfo2);
    }
    auto ui_textAntialiasModeSelector = makeUIObject<ComboBox>(5.0f);
    auto wk_textAntialiasModeSelector = (WeakPtr<ComboBox>)ui_textAntialiasModeSelector;

    std::vector<std::pair<Wstring, D2D1_TEXT_ANTIALIAS_MODE>> textAntialiasModePairs =
    {
        { L"Default", D2D1_TEXT_ANTIALIAS_MODE_DEFAULT },
        { L"Aliased", D2D1_TEXT_ANTIALIAS_MODE_ALIASED },
        { L"ClearType", D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE },
        { L"Grayscale", D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE }
    };
    using TextAntialiasModeMenuItemMap = std::unordered_map<Wstring, WeakPtr<MenuItem>>;
    TextAntialiasModeMenuItemMap textAntialiasModeMenuItems = {};
    {
        ui_textAntialiasModeSelector->setSize(200.0f, 40.0f);

        ConstraintLayout::GeometryInfo geoInfo1 = {};
        geoInfo1.Left.ToLeft = 130.0f;
        geoInfo1.Top.ToTop = 294.0f;
        ui_settingsLayout->addElement(ui_textAntialiasModeSelector, geoInfo1);

        PopupMenu::ItemArray strModeItems = {};
        for (auto& strMode : textAntialiasModePairs)
        {
            auto ui_content = IconLabel::comboBoxLayout(strMode.first);
            ui_content->label()->setTextFormat(D14_FONT(L"Default/14"));

            auto ui_menuItem = makeUIObject<MenuItem>(ui_content, math_utils::heightOnlyRect(40.0f));
            strModeItems.push_back(ui_menuItem);

            textAntialiasModeMenuItems[strMode.first] = ui_menuItem;
        }
        auto& dropDownMenu = ui_textAntialiasModeSelector->dropDownMenu();

        dropDownMenu->setSize(dropDownMenu->width(), 160.0f);
        dropDownMenu->appendItem(strModeItems);

        ui_textAntialiasModeSelector->setSelected(0);

        auto ui_label = makeUIObject<Label>(L"Text", math_utils::heightOnlyRect(40.0f));

        ui_label->setTextFormat(D14_FONT(L"Default/14"));
        ui_label->hardAlignment.vert = Label::VertAlignment::Center;

        ConstraintLayout::GeometryInfo geoInfo2 = {};
        geoInfo2.Left.ToLeft = 70.0f;
        geoInfo2.Top.ToTop = 294.0f;
        ui_settingsLayout->addElement(ui_label, geoInfo2);
    }
    auto ui_textRenderingModeLabel = makeUIObject<Label>(L"Text Rendering Mode");
    {
        ui_textRenderingModeLabel->setTextFormat(D14_FONT(L"Default/18"));
        ui_textRenderingModeLabel->hardAlignment.vert = Label::VertAlignment::Top;

        ConstraintLayout::GeometryInfo geoInfo = {};
        geoInfo.Left.ToLeft = 70.0f;
        geoInfo.Top.ToTop = 394.0f;
        ui_settingsLayout->addElement(ui_textRenderingModeLabel, geoInfo);
    }
    auto defTextRenderingMode = Application::g_app->renderer()->getDefaultTextRenderingMode();

    auto ui_gammaValueSlider = makeUIObject<HorzSlider>();
    auto wk_gammaValueSlider = (WeakPtr<HorzSlider>)ui_gammaValueSlider;
    {
        ui_gammaValueSlider->setSize(256.0f, 40.0f);

        auto& appear = ui_gammaValueSlider->appearance();
        appear.valueLabel.precision = 1;
        appear.valueLabel.resident = true;

        ui_gammaValueSlider->setMinValue(0.1f);
        ui_gammaValueSlider->setMaxValue(9.9f);
        ui_gammaValueSlider->setValue(defTextRenderingMode.gamma);

        ui_gammaValueSlider->stepMode = Slider::StepMode::Discrete;
        ui_gammaValueSlider->stepInterval = 0.1f;

        ConstraintLayout::GeometryInfo geoInfo1 = {};
        geoInfo1.Left.ToLeft = 240.0f;
        geoInfo1.Top.ToTop = 450.0f;
        ui_settingsLayout->addElement(ui_gammaValueSlider, geoInfo1);

        auto ui_label = makeUIObject<Label>(L"Gamma value", math_utils::heightOnlyRect(40.0f));

        ui_label->setTextFormat(D14_FONT(L"Default/14"));
        ui_label->hardAlignment.vert = Label::VertAlignment::Center;

        ConstraintLayout::GeometryInfo geoInfo2 = {};
        geoInfo2.Left.ToLeft = 70.0f;
        geoInfo2.Top.ToTop = 450.0f;
        ui_settingsLayout->addElement(ui_label, geoInfo2);
    }
    auto ui_enhancedContrastSlider = makeUIObject<HorzSlider>();
    auto wk_enhancedContrastSlider = (WeakPtr<HorzSlider>)ui_enhancedContrastSlider;
    {
        ui_enhancedContrastSlider->setSize(256.0f, 40.0f);

        auto& appear = ui_enhancedContrastSlider->appearance();
        appear.valueLabel.precision = 1;
        appear.valueLabel.resident = true;

        ui_enhancedContrastSlider->setMinValue(0.0f);
        ui_enhancedContrastSlider->setMaxValue(9.5f);
        ui_enhancedContrastSlider->setValue(defTextRenderingMode.enhancedContrast);

        ui_enhancedContrastSlider->stepMode = Slider::StepMode::Discrete;
        ui_enhancedContrastSlider->stepInterval = 0.5f;

        ConstraintLayout::GeometryInfo geoInfo1 = {};
        geoInfo1.Left.ToLeft = 240.0f;
        geoInfo1.Top.ToTop = 506.0f;
        ui_settingsLayout->addElement(ui_enhancedContrastSlider, geoInfo1);

        auto ui_label = makeUIObject<Label>(L"Enhanced contrast", math_utils::heightOnlyRect(40.0f));

        ui_label->setTextFormat(D14_FONT(L"Default/14"));
        ui_label->hardAlignment.vert = Label::VertAlignment::Center;

        ConstraintLayout::GeometryInfo geoInfo2 = {};
        geoInfo2.Left.ToLeft = 70.0f;
        geoInfo2.Top.ToTop = 506.0f;
        ui_settingsLayout->addElement(ui_label, geoInfo2);
    }
    auto ui_clearTypeLevelSlider = makeUIObject<HorzSlider>();
    auto wk_clearTypeLevelSlider = (WeakPtr<HorzSlider>)ui_clearTypeLevelSlider;
    {
        ui_clearTypeLevelSlider->setSize(256.0f, 40.0f);

        auto& appear = ui_clearTypeLevelSlider->appearance();
        appear.valueLabel.precision = 1;
        appear.valueLabel.resident = true;

        ui_clearTypeLevelSlider->setMinValue(0.0f);
        ui_clearTypeLevelSlider->setMaxValue(1.0f);
        ui_clearTypeLevelSlider->setValue(defTextRenderingMode.clearTypeLevel);

        ui_clearTypeLevelSlider->stepMode = Slider::StepMode::Discrete;
        ui_clearTypeLevelSlider->stepInterval = 0.1f;

        ConstraintLayout::GeometryInfo geoInfo1 = {};
        geoInfo1.Left.ToLeft = 240.0f;
        geoInfo1.Top.ToTop = 562.0f;
        ui_settingsLayout->addElement(ui_clearTypeLevelSlider, geoInfo1);

        auto ui_label = makeUIObject<Label>(L"ClearType level", math_utils::heightOnlyRect(40.0f));

        ui_label->setTextFormat(D14_FONT(L"Default/14"));
        ui_label->hardAlignment.vert = Label::VertAlignment::Center;

        ConstraintLayout::GeometryInfo geoInfo2 = {};
        geoInfo2.Left.ToLeft = 70.0f;
        geoInfo2.Top.ToTop = 562.0f;
        ui_settingsLayout->addElement(ui_label, geoInfo2);
    }
    std::vector<std::pair<Wstring, DWRITE_PIXEL_GEOMETRY>> pixelGeometryPairs =
    {
        { L"Flat", DWRITE_PIXEL_GEOMETRY_FLAT },
        { L"RGB", DWRITE_PIXEL_GEOMETRY_RGB },
        { L"BGR", DWRITE_PIXEL_GEOMETRY_BGR }
    };
    auto ui_pixelGeometrySelector = makeUIObject<ComboBox>(5.0f);
    auto wk_pixelGeometrySelector = (WeakPtr<ComboBox>)ui_pixelGeometrySelector;
    {
        ui_pixelGeometrySelector->setSize(200.0f, 40.0f);

        ConstraintLayout::GeometryInfo geoInfo1 = {};
        geoInfo1.Left.ToLeft = 240.0f;
        geoInfo1.Top.ToTop = 625.0f;
        ui_settingsLayout->addElement(ui_pixelGeometrySelector, geoInfo1);

        PopupMenu::ItemArray strModeItems = {};
        for (auto& strMode : pixelGeometryPairs)
        {
            auto ui_content = IconLabel::comboBoxLayout(strMode.first);
            ui_content->label()->setTextFormat(D14_FONT(L"Default/14"));

            strModeItems.push_back(makeUIObject<MenuItem>(ui_content, math_utils::heightOnlyRect(40.0f)));
        }
        auto& dropDownMenu = ui_pixelGeometrySelector->dropDownMenu();

        dropDownMenu->setSize(dropDownMenu->width(), 120.0f);
        dropDownMenu->appendItem(strModeItems);

        ui_pixelGeometrySelector->setSelected(1);

        auto ui_label = makeUIObject<Label>(L"Pixel geometry", math_utils::heightOnlyRect(40.0f));

        ui_label->setTextFormat(D14_FONT(L"Default/14"));
        ui_label->hardAlignment.vert = Label::VertAlignment::Center;

        ConstraintLayout::GeometryInfo geoInfo2 = {};
        geoInfo2.Left.ToLeft = 70.0f;
        geoInfo2.Top.ToTop = 625.0f;
        ui_settingsLayout->addElement(ui_label, geoInfo2);
    }
    std::vector<std::pair<Wstring, DWRITE_RENDERING_MODE>> renderingModePairs =
    {
        { L"Default", DWRITE_RENDERING_MODE_DEFAULT },
        { L"Aliased", DWRITE_RENDERING_MODE_ALIASED },
        { L"GDI Classic", DWRITE_RENDERING_MODE_GDI_CLASSIC },
        { L"GDI Natural", DWRITE_RENDERING_MODE_GDI_NATURAL },
        { L"Natural", DWRITE_RENDERING_MODE_NATURAL },
        { L"Natural Symmetric", DWRITE_RENDERING_MODE_NATURAL_SYMMETRIC },
        { L"Outline", DWRITE_RENDERING_MODE_OUTLINE }
    };
    auto ui_renderingModeSelector = makeUIObject<ComboBox>(5.0f);
    auto wk_renderingModeSelector = (WeakPtr<ComboBox>)ui_renderingModeSelector;

    using RenderingModeMenuItemMap = std::unordered_map<Wstring, WeakPtr<MenuItem>>;
    RenderingModeMenuItemMap renderingModeMenuItems = {};
    {
        ui_renderingModeSelector->setSize(200.0f, 40.0f);

        ConstraintLayout::GeometryInfo geoInfo1 = {};
        geoInfo1.Left.ToLeft = 240.0f;
        geoInfo1.Top.ToTop = 685.0f;
        ui_settingsLayout->addElement(ui_renderingModeSelector, geoInfo1);

        PopupMenu::ItemArray strModeItems = {};
        for (auto& strMode : renderingModePairs)
        {
            auto ui_content = IconLabel::comboBoxLayout(strMode.first);
            ui_content->label()->setTextFormat(D14_FONT(L"Default/14"));

            auto ui_menuItem = makeUIObject<MenuItem>(ui_content, math_utils::heightOnlyRect(40.0f));
            strModeItems.push_back(ui_menuItem);

            renderingModeMenuItems[strMode.first] = ui_menuItem;
        }
        auto& dropDownMenu = ui_renderingModeSelector->dropDownMenu();

        dropDownMenu->setSize(dropDownMenu->width(), 280.0f);
        dropDownMenu->appendItem(strModeItems);

        ui_renderingModeSelector->setSelected(0);

        auto ui_label = makeUIObject<Label>(L"Rendering mode", math_utils::heightOnlyRect(40.0f));

        ui_label->setTextFormat(D14_FONT(L"Default/14"));
        ui_label->hardAlignment.vert = Label::VertAlignment::Center;

        ConstraintLayout::GeometryInfo geoInfo2 = {};
        geoInfo2.Left.ToLeft = 70.0f;
        geoInfo2.Top.ToTop = 685.0f;
        ui_settingsLayout->addElement(ui_label, geoInfo2);
    }
    using PixelGeometryMap = std::unordered_map<Wstring, DWRITE_PIXEL_GEOMETRY>;
    auto pixelGeometryMap = std::make_shared<PixelGeometryMap>
    (
        pixelGeometryPairs.begin(), pixelGeometryPairs.end()
    );
    using RenderingModeMap = std::unordered_map<Wstring, DWRITE_RENDERING_MODE>;
    auto renderingModeMap = std::make_shared<RenderingModeMap>
    (
        renderingModePairs.begin(), renderingModePairs.end()
    );
    auto changeTextRenderingMode = [=]
    {
        if (!wk_gammaValueSlider.expired() &&
            !wk_enhancedContrastSlider.expired() &&
            !wk_clearTypeLevelSlider.expired() &&
            !wk_pixelGeometrySelector.expired() &&
            !wk_renderingModeSelector.expired())
        {
            auto sh_gammaValueSlider = wk_gammaValueSlider.lock();
            auto sh_enhancedContrastSlider = wk_enhancedContrastSlider.lock();
            auto sh_clearTypeLevelSlider = wk_clearTypeLevelSlider.lock();
            auto sh_pixelGeometrySelector = wk_pixelGeometrySelector.lock();
            auto sh_renderingModeSelector = wk_renderingModeSelector.lock();

            auto& pixelGeoStr = sh_pixelGeometrySelector->content()->label()->text();
            auto& rndrModeStr = sh_renderingModeSelector->content()->label()->text();

            Renderer::TextRenderingSettings settings = {};

            settings.gamma = sh_gammaValueSlider->value();
            settings.enhancedContrast = sh_enhancedContrastSlider->value();
            settings.clearTypeLevel = sh_clearTypeLevelSlider->value();
            settings.pixelGeometry = pixelGeometryMap->at(pixelGeoStr);
            settings.renderingMode = renderingModeMap->at(rndrModeStr);

            Application::g_app->renderer()->setTextRenderingMode(settings);
        }
    };
    ui_gammaValueSlider->f_onValueChange =
    ui_enhancedContrastSlider->f_onValueChange =
    ui_clearTypeLevelSlider->f_onValueChange =
    [=](Slider::ValuefulObject* obj, float value)
    {
        changeTextRenderingMode();
    };
    ui_pixelGeometrySelector->f_onSelectedChange =
    [=](ComboBox* cb, OptRefer<size_t> index)
    {
        changeTextRenderingMode();
    };
    ui_renderingModeSelector->f_onSelectedChange =
    [=](ComboBox* cb, OptRefer<size_t> index)
    {
        changeTextRenderingMode();

        // Text Rendering Mode Conflict:
        //
        // DWRITE_RENDERING_MODE_ALIASED:
        //     D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE
        //     D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE
        //
        // DWRITE_RENDERING_MODE_GDI_CLASSIC:
        //     D2D1_TEXT_ANTIALIAS_MODE_ALIASED
        //
        // DWRITE_RENDERING_MODE_GDI_NATURAL:
        //     D2D1_TEXT_ANTIALIAS_MODE_ALIASED
        //
        // DWRITE_RENDERING_MODE_NATURAL:
        //     D2D1_TEXT_ANTIALIAS_MODE_ALIASED
        //
        // DWRITE_RENDERING_MODE_NATURAL_SYMMETRIC:
        //     D2D1_TEXT_ANTIALIAS_MODE_ALIASED
        //
        // DWRITE_RENDERING_MODE_OUTLINE:
        //     D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE

        static auto updateConflictMenuItems = [&](std::set<Wstring> conflictNames)
        {
            for (auto& kv : textAntialiasModeMenuItems)
            {
                if (!kv.second.expired())
                {
                    auto conflictMenuItem = kv.second.lock();
                    if (conflictNames.find(kv.first) != conflictNames.end())
                    {
                        conflictMenuItem->setEnabled(false);
                    }
                    else conflictMenuItem->setEnabled(true);
                }
            }
        };
        auto& modeStr = cb->content()->label()->text();
        if (modeStr == L"Aliased")
        {
            updateConflictMenuItems({ L"ClearType", L"Grayscale" });
        }
        else if (modeStr == L"GDI Classic" || modeStr == L"GDI Natural" ||
                 modeStr == L"Natural" || modeStr == L"Natural Symmetric")
        {
            updateConflictMenuItems({ L"Aliased" });
        }
        else if (modeStr == L"Outline")
        {
            updateConflictMenuItems({ L"ClearType" });
        }
    };
    using TextAntialiasModeMap = std::unordered_map<Wstring, D2D1_TEXT_ANTIALIAS_MODE>;
    ui_textAntialiasModeSelector->f_onSelectedChange =
    [=,
        textAntialiasModeMap = TextAntialiasModeMap{ textAntialiasModePairs.begin(), textAntialiasModePairs.end() }
    ]
    (ComboBox* cb, OptRefer<size_t> index)
    {
        // Text Antialias Mode Conflict:
        //
        // D2D1_TEXT_ANTIALIAS_MODE_ALIASED:
        //     DWRITE_RENDERING_MODE_GDI_CLASSIC
        //     DWRITE_RENDERING_MODE_GDI_NATURAL
        //     DWRITE_RENDERING_MODE_NATURAL
        //     DWRITE_RENDERING_MODE_NATURAL_SYMMETRIC
        //
        // D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE:
        //     DWRITE_RENDERING_MODE_ALIASED
        //     DWRITE_RENDERING_MODE_OUTLINE
        //
        // D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE:
        //     DWRITE_RENDERING_MODE_ALIASED

        static auto updateConflictMenuItems = [&](std::set<Wstring> conflictNames)
        {
            for (auto& kv : renderingModeMenuItems)
            {
                if (!kv.second.expired())
                {
                    auto conflictMenuItem = kv.second.lock();
                    if (conflictNames.find(kv.first) != conflictNames.end())
                    {
                        conflictMenuItem->setEnabled(false);
                    }
                    else conflictMenuItem->setEnabled(true);
                }
            }
        };
        auto& modeStr = cb->content()->label()->text();
        if (modeStr == L"Aliased")
        {
            updateConflictMenuItems({ L"GDI Classic", L"GDI Natural", L"Natural", L"Natural Symmetric" });
        }
        else if (modeStr == L"ClearType")
        {
            updateConflictMenuItems({ L"Aliased", L"Outline" });
        }
        else if (modeStr == L"Grayscale")
        {
            updateConflictMenuItems({ L"Aliased"});
        }
        Application::g_app->renderer()->setTextAntialiasMode(textAntialiasModeMap.at(modeStr));
    };
    ui_textAntialiasModeSelector->setSelected(2); // Antialias ClearType
    ui_renderingModeSelector->setSelected(5); // Natural Symmetric

    auto ui_frameRateSettingLabel = makeUIObject<Label>(L"Frame Rate Setting");
    {
        ui_frameRateSettingLabel->setTextFormat(D14_FONT(L"Default/18"));
        ui_frameRateSettingLabel->hardAlignment.vert = Label::VertAlignment::Top;

        ConstraintLayout::GeometryInfo geoInfo = {};
        geoInfo.Left.ToLeft = 70.0f;
        geoInfo.Top.ToTop = 785.0f;
        ui_settingsLayout->addElement(ui_frameRateSettingLabel, geoInfo);
    }
    auto ui_fullSpeedRenderingSwitch = makeUIObject<OnOffSwitch>();
    {
        ConstraintLayout::GeometryInfo geoInfo1 = {};
        geoInfo1.Left.ToLeft = 70.0f;
        geoInfo1.Top.ToTop = 834.0f;
        ui_settingsLayout->addElement(ui_fullSpeedRenderingSwitch, geoInfo1);

        auto ui_label = makeManagedUIObject<Label>(ui_fullSpeedRenderingSwitch, L"Full speed rendering");
        auto wk_label = (WeakPtr<Label>)ui_label;

        ui_fullSpeedRenderingSwitch->reorderAbovePeerObject(ui_label.get());

        ui_label->transform(63.0f, 0.0f, 0.0f, ui_darkModeSwitch->height());
        ui_label->setTextFormat(D14_FONT(L"Default/14"));
        ui_label->hardAlignment.vert = Label::VertAlignment::Center;

        ui_fullSpeedRenderingSwitch->f_isHit = [=](const Panel* p, const Event::Point& pt)
        {
            float horzOffset = 0.0f;
            if (!wk_label.expired()) horzOffset = 15.0f + wk_label.lock()->textAreaSize().width;
            return math_utils::isOverlapped(pt, math_utils::increaseRight(p->absoluteRect(), horzOffset));
        };
    }
    auto ui_frameRateLimitLabel = makeUIObject<Label>(L"Frame rate limit");
    auto wk_frameRateLimitLabel = (WeakPtr<Label>)ui_frameRateLimitLabel;
    auto ui_frameRateLimitInput = makeUIObject<RawTextBox>(5.0f);
    auto wk_frameRateLimitInput = (WeakPtr<RawTextBox>)ui_frameRateLimitInput;
    auto ui_realTimeFpsLabel = makeUIObject<Label>(L"Real-time FPS: None");
    auto wk_realTimeFpsLabel = (WeakPtr<Label>)ui_realTimeFpsLabel;
    {
        ui_frameRateLimitLabel->setSize(0.0f, 40.0f);

        ui_frameRateLimitLabel->setTextFormat(D14_FONT(L"Default/14"));
        ui_frameRateLimitLabel->hardAlignment.vert = Label::VertAlignment::Center;

        ConstraintLayout::GeometryInfo geoInfo1 = {};
        geoInfo1.Left.ToLeft = 70.0f;
        geoInfo1.Top.ToTop = 878.0f;
        ui_settingsLayout->addElement(ui_frameRateLimitLabel, geoInfo1);

        ui_frameRateLimitInput->setSize(80.0f, 40.0f);

        ui_frameRateLimitInput->setTextFormat(D14_FONT(L"Default/14"));
        ui_frameRateLimitInput->setVisibleTextRect({ 5.0f, 8.0f, 75.0f, 32.0f });
        ui_frameRateLimitInput->placeholder()->setText(L"1 ~ 999");
        ui_frameRateLimitInput->placeholder()->setTextFormat(D14_FONT(L"Default/14"));

        ConstraintLayout::GeometryInfo geoInfo2 = {};
        geoInfo2.Left.ToLeft = 215.0f;
        geoInfo2.Top.ToTop = 878.0f;
        ui_settingsLayout->addElement(ui_frameRateLimitInput, geoInfo2);

        ui_realTimeFpsLabel->setSize(0.0f, 40.0f);

        ui_realTimeFpsLabel->setTextFormat(D14_FONT(L"Default/14"));
        ui_realTimeFpsLabel->hardAlignment.vert = Label::VertAlignment::Center;

        ConstraintLayout::GeometryInfo geoInfo3 = {};
        geoInfo3.Left.ToLeft = 310.0f;
        geoInfo3.Top.ToTop = 878.0f;
        ui_settingsLayout->addElement(ui_realTimeFpsLabel, geoInfo3);

        ui_fullSpeedRenderingSwitch->f_onStateChange = [=]
        (OnOffSwitch::StatefulObject* obj, OnOffSwitch::StatefulObject::Event& e)
        {
            if (!wk_frameRateLimitLabel.expired()) wk_frameRateLimitLabel.lock()->setEnabled(e.on());
            if (!wk_frameRateLimitInput.expired()) wk_frameRateLimitInput.lock()->setEnabled(e.on());
            if (!wk_realTimeFpsLabel.expired()) wk_realTimeFpsLabel.lock()->setEnabled(e.on());
            if (e.off())
            {
                if (!wk_frameRateLimitInput.expired()) wk_frameRateLimitInput.lock()->setText(L"");
                if (!wk_realTimeFpsLabel.expired()) wk_realTimeFpsLabel.lock()->setText(L"Real-time FPS: None");
            }
            if (e.on()) Application::g_app->increaseAnimationCount();
            else if (e.off()) Application::g_app->decreaseAnimationCount();
        };
        ui_frameRateLimitInput->f_onLoseKeyboardFocus = [](Panel* p)
        {
            auto label = (Label*)p;
            int fpsLimit = _wtoi(label->text().c_str());
            label->setText(std::to_wstring(std::clamp(fpsLimit, 1, 999)) + L" fps");

            // TODO: Add changing frame rate limit callback.
        };
        ui_realTimeFpsLabel->f_onRendererUpdateObject2DAfter = [](Panel* p, Renderer* rndr)
        {
            if (p->enabled()) ((Label*)p)->setText(L"Real-time FPS: " + std::to_wstring(rndr->timer()->fpsNum()));
        };
    }
}
