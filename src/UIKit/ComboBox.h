﻿#pragma once

#include "Common/Precompile.h"

#include "UIKit/Appearances/ComboBox.h"
#include "UIKit/FlatButton.h"

namespace d14engine::uikit
{
    struct IconLabel;
    struct MenuItem;
    struct PopupMenu;

    struct ComboBox : appearance::ComboBox, FlatButton
    {
        // The typical height is 40, i.e. math_utils::height(rect) == 40.
        ComboBox(float roundRadius = 0.0f, const D2D1_RECT_F& rect = {});

        void onInitializeFinish() override;

        _D14_SET_APPEARANCE_PROPERTY(ComboBox)

        //////////////////////
        // Cached Resources //
        //////////////////////

        using MasterPtr = cpp_lang_utils::EnableMasterPtr<ComboBox>;

        struct ArrowIcon : MasterPtr
        {
            using MasterPtr::MasterPtr;

            ComPtr<ID2D1StrokeStyle> strokeStyle = {};
            void loadStrokeStyle();
        }
        arrowIcon{ this };

        ////////////////////////
        // Callback Functions //
        ////////////////////////

        //------------------------------------------------------------------
        // Public Interfaces
        //------------------------------------------------------------------
    public:
        void onSelectedChange(OptRefer<size_t> index);

        Function<void(ComboBox*, OptRefer<size_t>)> f_onSelectedChange = {};

        //------------------------------------------------------------------
        // Protected Helpers
        //------------------------------------------------------------------
    protected:
        void onSelectedChangeHelper(OptRefer<size_t> index);

        //////////////////////////
        // Graphical Components //
        //////////////////////////

        //------------------------------------------------------------------
        // Drop-Down Menu
        //------------------------------------------------------------------
    protected:
        SharedPtr<PopupMenu> m_dropDownMenu = {};

    public:
        ShrdPtrRefer<PopupMenu> dropDownMenu() const;
        void setDropDownMenu(ShrdPtrRefer<PopupMenu> menu);

        ///////////////////////
        // Interaction Logic //
        ///////////////////////

        //------------------------------------------------------------------
        // Menu Offset
        //------------------------------------------------------------------
    public:
        Optional<D2D1_POINT_2F> menuOffset = {};

        //------------------------------------------------------------------
        // Select Item
        //------------------------------------------------------------------
    protected:
        Optional<size_t> m_selectedIndex = {};

    public:
        OptRefer<size_t> selectedIndex() const;
        void setSelected(OptRefer<size_t> index);

        /////////////////////////
        // Interface Overrides //
        /////////////////////////

    protected:
        //------------------------------------------------------------------
        // IDrawObject2D
        //------------------------------------------------------------------

        void onRendererDrawD2d1ObjectHelper(Renderer* rndr) override;

        //------------------------------------------------------------------
        // Panel
        //------------------------------------------------------------------

        void onSizeHelper(SizeEvent& e) override;

        void onChangeThemeStyleHelper(const ThemeStyle& style) override;

        //------------------------------------------------------------------
        // ClickablePanel
        //------------------------------------------------------------------

        void onMouseButtonReleaseHelper(ClickablePanel::Event& e) override;
    };
}
