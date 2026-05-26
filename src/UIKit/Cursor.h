#pragma once

#include "Common/Precompile.h"

#include "UIKit/AnimationUtils/BitmapSequence.h"
#include "UIKit/Panel.h"

namespace d14engine::uikit
{
    struct Application;

    struct Cursor : Panel
    {
        friend Application;

        //////////////////////
        // Type Definitions //
        //////////////////////

        //------------------------------------------------------------------
        // Icon Index
        //------------------------------------------------------------------

        enum class StaticIconIndex
        {
            Alternate, Arrow, BackDiag, Hand, Help, HorzSize,
            MainDiag, Move, Pen, Person, Pin, Select, Stop, Text, VertSize
        };
        enum class DynamicIconIndex
        {
            Busy, Working
        };
#define SET_STATIC_ALIAS(Name) constexpr static auto Name = StaticIconIndex::Name;
#define SET_DYNAMIC_ALIAS(Name) constexpr static auto Name = DynamicIconIndex::Name;

        SET_STATIC_ALIAS(Alternate)
        SET_STATIC_ALIAS(Arrow)
        SET_STATIC_ALIAS(BackDiag)
        SET_STATIC_ALIAS(Hand)
        SET_STATIC_ALIAS(Help)
        SET_STATIC_ALIAS(HorzSize)
        SET_STATIC_ALIAS(MainDiag)
        SET_STATIC_ALIAS(Move)
        SET_STATIC_ALIAS(Pen)
        SET_STATIC_ALIAS(Person)
        SET_STATIC_ALIAS(Pin)
        SET_STATIC_ALIAS(Select)
        SET_STATIC_ALIAS(Stop)
        SET_STATIC_ALIAS(Text)
        SET_STATIC_ALIAS(VertSize)

        SET_DYNAMIC_ALIAS(Busy)
        SET_DYNAMIC_ALIAS(Working)

#undef SET_STATIC_ALIAS
#undef SET_DYNAMIC_ALIAS

        //------------------------------------------------------------------
        // Icon Types
        //------------------------------------------------------------------

        template<typename BitmapData>
        struct Icon
        {
            BitmapData bitmapData = {}; D2D1_POINT_2F hotSpotOffset = {};
        };
        using StaticIcon = Icon<BitmapObject>;
        using DynamicIcon = Icon<animation_utils::BitmapSequence>;

        using StaticIconMap = cpp_lang_utils::EnumMap<StaticIconIndex, StaticIcon>;
        using DynamicIconMap = cpp_lang_utils::EnumMap<DynamicIconIndex, DynamicIcon>;

        struct ThemeIcon
        {
            StaticIconMap staticIcons = {};
            DynamicIconMap dynamicIcons = {};
        };
        using ThemeIconLibrary = std::unordered_map<Wstring, ThemeIcon>;

        //------------------------------------------------------------------
        // Initialization
        //------------------------------------------------------------------

        Cursor(
            const D2D1_RECT_F& rect = { 0.0f, 0.0f, 32.0f, 32.0f },
            const ThemeIconLibrary& iconLib = loadThemeIconLibrary());

        void initialize() override;

        ///////////////////////
        // Interaction Logic //
        ///////////////////////

        //------------------------------------------------------------------
        // Theme Icons
        //------------------------------------------------------------------
    protected:
        ThemeIconLibrary m_themeIconLibrary = {};

        static ThemeIconLibrary loadThemeIconLibrary();

        static ThemeIcon loadThemeIcon(WstrParam themeName);
        static DynamicIcon loadDynamicIcon(WstrParam imagePath);

    public:
        void registerThemeIcon(WstrParam themeName, const ThemeIcon& icon);
        void unregisterThemeIcon(WstrParam themeName);

        //------------------------------------------------------------------
        // Named Icons
        //------------------------------------------------------------------
    protected:
        using NamedIcon = Variant<StaticIcon, DynamicIcon>;
        using NamedIconLibrary = std::unordered_map<Wstring, NamedIcon>;

        NamedIconLibrary m_namedIconLibrary = {};

    public:
        void registerNamedIcon(WstrParam iconName, const NamedIcon& icon);
        void unregisterNamedIcon(WstrParam iconName);

        //------------------------------------------------------------------
        // Select Icon
        //------------------------------------------------------------------
    protected:
        template<typename T>
        using IconID = Variant<StaticIconIndex, DynamicIconIndex, T>;

        using IconIDData = IconID<Wstring>;
        using IconIDView = IconID<WstringView>;

        IconIDData m_selectedIconID = Arrow;
        IconIDData m_lastSelectedIconID = Arrow;

    public:
        void setIcon(const IconIDView& iconID);

        Optional<Wstring> customIconTheme = {};

        //------------------------------------------------------------------
        // Draw Backend
        //------------------------------------------------------------------
    public:
        enum class DrawBackend { System, UIKit };

        constexpr static auto System = DrawBackend::System;
        constexpr static auto UIKit = DrawBackend::UIKit;

    protected:
        DrawBackend m_drawBackend = System;

    public:
        DrawBackend drawBackend() const;
        void setDrawBackend(DrawBackend backend);

        //------------------------------------------------------------------
        // Miscellaneous
        //------------------------------------------------------------------
    protected:
        // Displays the corresponding system built-in cursor
        // related to the currently selected theme icon index.
        void setSystemIcon();

        bool m_hasPendingSetCursorMessage = false;

    protected:
        using IconObject = Variant<Ref<StaticIcon>, Ref<DynamicIcon>>;

        IconObject getIconObject(const IconIDData& iconIDData);

        /////////////////////////
        // Interface Overrides //
        /////////////////////////

    protected:
        //------------------------------------------------------------------
        // IDrawObject2D
        //------------------------------------------------------------------

        void onRendererUpdateObject2DHelper(Renderer* rndr) override;

        void onRendererDrawD2d1ObjectHelper(Renderer* rndr) override;
    };
}
