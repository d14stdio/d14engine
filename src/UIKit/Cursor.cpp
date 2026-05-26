#include "Common/Precompile.h"

#include "UIKit/Cursor.h"

#include "Common/CppLangUtils/EnumMagic.h"
#include "Common/MathUtils/2D.h"

#include "UIKit/BitmapUtils.h"
#include "UIKit/FileSystemUtils.h"
#include "UIKit/PlatformUtils.h"
#include "UIKit/ResourceUtils.h"

using namespace d14engine::renderer;
using namespace fanim_literals;

namespace d14engine::uikit
{
    Cursor::Cursor(const D2D1_RECT_F& rect, const ThemeIconLibrary& iconLib)
        :
        Panel(rect),
        m_themeIconLibrary(iconLib)
    {
        // Here left blank intentionally.
    }

    void Cursor::initialize()
    {
        Panel::initialize();

        THROW_IF_NULL(Application::g_app);

        // Keep the cursor always displayed at the top.
        ISortable<IDrawObject2D>::m_priority = INT_MAX;

        Application::g_app->drawObjects().insert(shared_from_this());
    }

    Cursor::ThemeIconLibrary Cursor::loadThemeIconLibrary()
    {
        return
        {
            { L"Light", loadThemeIcon(L"Light") },
            { L"Dark",  loadThemeIcon(L"Dark") }
        };
    }

    Cursor::ThemeIcon Cursor::loadThemeIcon(WstrParam themeName)
    {
        THROW_IF_NULL(Application::g_app);

        ThemeIcon icons = {};

        auto cursorPath = Application::g_app->createInfo.cursorPath() + themeName + L"/";

        ///////////////////////
        // Load Static Icons //
        ///////////////////////

#define SET_STATIC_ICON(Index, Name, ...) \
do { \
    staticIconInfo[(size_t)StaticIconIndex::Index] = { L#Name L".png", __VA_ARGS__ }; \
} while (0)

        using StaticIconInfo = std::pair<Wstring, D2D1_POINT_2F>;
        using StaticIconInfoMap = cpp_lang_utils::EnumMap<StaticIconIndex, StaticIconInfo>;

        StaticIconInfoMap staticIconInfo = {};
        if (themeName == L"Light")
        {
            SET_STATIC_ICON(Alternate, alternate,   { 33.0f, 8.0f });
            SET_STATIC_ICON(Arrow,     pointer,     { 6.0f, 18.0f });
            SET_STATIC_ICON(BackDiag,  dgn2,        { 31.0f, 31.0f });
            SET_STATIC_ICON(Hand,      link,        { 25.0f, 14.0f });
            SET_STATIC_ICON(Help,      help,        { 6.0f, 18.0f });
            SET_STATIC_ICON(HorzSize,  horz,        { 31.0f, 31.0f });
            SET_STATIC_ICON(MainDiag,  dgn1,        { 31.0f, 31.0f });
            SET_STATIC_ICON(Move,      move,        { 31.0f, 31.0f });
            SET_STATIC_ICON(Pen,       handwriting, { 10.0f, 14.0f });
            SET_STATIC_ICON(Person,    person,      { 21.0f, 14.0f });
            SET_STATIC_ICON(Pin,       pin,         { 21.0f, 14.0f });
            SET_STATIC_ICON(Select,    precision,   { 32.0f, 32.0f });
            SET_STATIC_ICON(Stop,      unavailable, { 31.0f, 31.0f });
            SET_STATIC_ICON(Text,      beam,        { 31.0f, 31.0f });
            SET_STATIC_ICON(VertSize,  vert,        { 31.0f, 31.0f });
        }
        else if (themeName == L"Dark")
        {
            SET_STATIC_ICON(Alternate, alternate,   { 33.0f, 8.0f });
            SET_STATIC_ICON(Arrow,     pointer,     { 6.0f, 18.0f });
            SET_STATIC_ICON(BackDiag,  dgn2,        { 31.0f, 31.0f });
            SET_STATIC_ICON(Hand,      link,        { 25.0f, 14.0f });
            SET_STATIC_ICON(Help,      help,        { 6.0f, 18.0f });
            SET_STATIC_ICON(HorzSize,  horz,        { 31.0f, 31.0f });
            SET_STATIC_ICON(MainDiag,  dgn1,        { 31.0f, 31.0f });
            SET_STATIC_ICON(Move,      move,        { 31.0f, 31.0f });
            SET_STATIC_ICON(Pen,       handwriting, { 12.0f, 16.0f });
            SET_STATIC_ICON(Person,    person,      { 21.0f, 14.0f });
            SET_STATIC_ICON(Pin,       pin,         { 21.0f, 14.0f });
            SET_STATIC_ICON(Select,    precision,   { 32.0f, 32.0f });
            SET_STATIC_ICON(Stop,      unavailable, { 31.0f, 31.0f });
            SET_STATIC_ICON(Text,      beam,        { 31.0f, 31.0f });
            SET_STATIC_ICON(VertSize,  vert,        { 31.0f, 31.0f });
        }

#undef SET_STATIC_ICON

        for (size_t i = 0; i < staticIconInfo.size(); ++i)
        {
            auto& info = staticIconInfo[i];
            icons.staticIcons[i] =
            {
                bitmap_utils::loadBitmap(cursorPath + info.first),
                platform_utils::restoredByDpi(info.second)
            };
        }

        ////////////////////////
        // Load Dynamic Icons //
        ////////////////////////

#define LOAD_DYNAMIC_ICON(Index, Name, ...) \
do { \
    auto frames = loadDynamicIcon(cursorPath + L#Name L"/"); \
    frames.hotSpotOffset = __VA_ARGS__; \
    icons.dynamicIcons[(size_t)DynamicIconIndex::Index] = std::move(frames); \
} while (0)

        if (themeName == L"Light")
        {
            LOAD_DYNAMIC_ICON(Busy,    busy,    { 31.0f, 31.0f });
            LOAD_DYNAMIC_ICON(Working, working, { 6.0f, 18.0f });
        }
        else if (themeName == L"Dark")
        {
            LOAD_DYNAMIC_ICON(Busy,    busy,    { 31.0f, 31.0f });
            LOAD_DYNAMIC_ICON(Working, working, { 6.0f, 18.0f });
        }

#undef LOAD_DYNAMIC_ICON

        return icons;
    }

    Cursor::DynamicIcon Cursor::loadDynamicIcon(WstrParam imagePath)
    {
        DynamicIcon icon = {};

        /////////////////
        // Load Images //
        /////////////////

        animation_utils::BitmapSequence::FrameMap frames = {};

        file_system_utils::foreachFileInDir
        (imagePath, L"*.png", [&](WstrParam path)
        {
            auto name = file_system_utils::extractFilePrefix(
                        file_system_utils::extractFileName(path));

            frames[name] = bitmap_utils::loadBitmap(path);

            return false;
        });

        //////////////////
        // Setup Frames //
        //////////////////

        auto& fanim = icon.bitmapData.fanim;

        fanim.frames.resize(frames.size());
        for (auto& f : frames)
        {
            auto index = (size_t)(std::stoi(f.first) - 1);
            if (index >= 0_uz && index < fanim.frames.size())
            {
                fanim.frames[index] = f.second;
            }
        }
        fanim.timeSpanDataInSecs = 2_jf;

        return icon;
    }

    void Cursor::registerThemeIcon(WstrParam themeName, const ThemeIcon& icon)
    {
        auto iconItor = m_themeIconLibrary.find(themeName);
        if (iconItor != m_themeIconLibrary.end())
        {
            iconItor->second = icon;
        }
        else m_themeIconLibrary[themeName] = icon;
    }

    void Cursor::unregisterThemeIcon(WstrParam themeName)
    {
        m_themeIconLibrary.erase(themeName);
    }

    void Cursor::registerNamedIcon(WstrParam iconName, const NamedIcon& icon)
    {
        auto iconItor = m_namedIconLibrary.find(iconName);
        if (iconItor != m_namedIconLibrary.end())
        {
            iconItor->second = icon;
        }
        else m_namedIconLibrary[iconName] = icon;
    }

    void Cursor::unregisterNamedIcon(WstrParam iconName)
    {
        m_namedIconLibrary.erase(iconName);
    }

    void Cursor::setIcon(const IconIDView& iconID)
    {
        THROW_IF_NULL(Application::g_app);

        if (std::holds_alternative<WstringView>(iconID))
        {
            m_selectedIconID = (Wstring)std::get<WstringView>(iconID);
        }
        else if (std::holds_alternative<StaticIconIndex>(iconID))
        {
            m_selectedIconID = std::get<StaticIconIndex>(iconID);
        }
        else if (std::holds_alternative<DynamicIconIndex>(iconID))
        {
            m_selectedIconID = std::get<DynamicIconIndex>(iconID);
        }
        else THROW_ERROR(L"Invalid IconID type: Cursor::setIcon.");

        if (m_drawBackend == System && !m_hasPendingSetCursorMessage)
        {
            m_hasPendingSetCursorMessage = true;
            PostMessage(Application::g_app->win32Window(), WM_SETCURSOR, 0, HTCLIENT);
        }
    }

    Cursor::DrawBackend Cursor::drawBackend() const
    {
        return m_drawBackend;
    }

    void Cursor::setDrawBackend(DrawBackend backend)
    {
        if ((m_drawBackend = backend) == System)
        {
            PostMessage(Application::g_app->win32Window(), WM_SETCURSOR, 0, HTCLIENT);
        }
    }

    Cursor::IconObject Cursor::getIconObject(const IconIDData& iconIDData)
    {
        THROW_IF_NULL(Application::g_app);

        auto& app = Application::g_app;

        if (std::holds_alternative<Wstring>(iconIDData))
        {
            auto& iconID = std::get<Wstring>(iconIDData);
            auto& namedIcon = m_namedIconLibrary.at(iconID);
            if (std::holds_alternative<StaticIcon>(namedIcon))
            {
                return std::get<StaticIcon>(namedIcon);
            }
            else if (std::holds_alternative<DynamicIcon>(namedIcon))
            {
                return std::get<DynamicIcon>(namedIcon);
            }
            else THROW_ERROR(L"Invalid IconID type: Cursor::getIconObject");
        }
        else if (std::holds_alternative<StaticIconIndex>(iconIDData))
        {
            auto& iconID = std::get<StaticIconIndex>(iconIDData);
            auto& themeIcon = m_themeIconLibrary.at(app->themeStyle().name);
            return themeIcon.staticIcons.at((size_t)iconID);
        }
        else if (std::holds_alternative<DynamicIconIndex>(iconIDData))
        {
            auto& iconID = std::get<DynamicIconIndex>(iconIDData);
            auto& themeIcon = m_themeIconLibrary.at(app->themeStyle().name);
            return themeIcon.dynamicIcons.at((size_t)iconID);
        }
        else THROW_ERROR(L"Invalid IconID type: Cursor::getIconObject");
    }

    void Cursor::setSystemIcon()
    {
        if (m_visible)
        {
#define SET_CURSOR(Icon_Name) SetCursor(LoadCursor(nullptr, Icon_Name)); break

            if (std::holds_alternative<Wstring>(m_selectedIconID))
            {
                SetCursor(nullptr);
            }
            else if (std::holds_alternative<StaticIconIndex>(m_selectedIconID))
            {
                switch (std::get<StaticIconIndex>(m_selectedIconID))
                {
                case Alternate: SET_CURSOR(IDC_UPARROW);
                case Arrow:     SET_CURSOR(IDC_ARROW);
                case BackDiag:  SET_CURSOR(IDC_SIZENESW);
                case Hand:      SET_CURSOR(IDC_HAND);
                case Help:      SET_CURSOR(IDC_HELP);
                case HorzSize:  SET_CURSOR(IDC_SIZEWE);
                case MainDiag:  SET_CURSOR(IDC_SIZENWSE);
                case Move:      SET_CURSOR(IDC_SIZEALL);
                case Person:    SET_CURSOR(IDC_PERSON);
                case Pin:       SET_CURSOR(IDC_PIN);
                case Select:    SET_CURSOR(IDC_CROSS);
                case Stop:      SET_CURSOR(IDC_NO);
                case Text:      SET_CURSOR(IDC_IBEAM);
                case VertSize:  SET_CURSOR(IDC_SIZENS);
                default: SetCursor(nullptr); break;
                }
            }
            else if (std::holds_alternative<DynamicIconIndex>(m_selectedIconID))
            {
                switch (std::get<DynamicIconIndex>(m_selectedIconID))
                {
                case Busy:    SET_CURSOR(IDC_WAIT);
                case Working: SET_CURSOR(IDC_APPSTARTING);
                default: SetCursor(nullptr); break;
                }
            }
#undef SET_CURSOR
        }
        else SetCursor(nullptr);
    }

    void Cursor::onRendererUpdateObject2DHelper(Renderer* rndr)
    {
        if (m_drawBackend == UIKit)
        {
            auto iconObj = getIconObject(m_selectedIconID);
            if (std::holds_alternative<Ref<DynamicIcon>>(iconObj))
            {
                auto& icon = std::get<Ref<DynamicIcon>>(iconObj).get();
                icon.bitmapData.update(rndr);
            }
        }
    }

    void Cursor::onRendererDrawD2d1ObjectHelper(Renderer* rndr)
    {
        if (m_drawBackend == UIKit)
        {
            auto iconObj = getIconObject(m_selectedIconID);
            auto lastIconObj = getIconObject(m_lastSelectedIconID);

            if (std::holds_alternative<Ref<DynamicIcon>>(iconObj) &&
                std::holds_alternative<Ref<StaticIcon>>(lastIconObj))
            {
                auto& icon = std::get<Ref<DynamicIcon>>(iconObj).get();

                icon.bitmapData.restore();
                increaseAnimationCount();
            }
            if (std::holds_alternative<Ref<StaticIcon>>(iconObj) &&
                std::holds_alternative<Ref<DynamicIcon>>(lastIconObj))
            {
                decreaseAnimationCount();
            }
            m_lastSelectedIconID = m_selectedIconID;

            if (std::holds_alternative<Ref<StaticIcon>>(iconObj))
            {
                auto& icon = std::get<Ref<StaticIcon>>(iconObj).get();

                auto hs = math_utils::minus(icon.hotSpotOffset);
                auto rect = math_utils::offset(m_absoluteRect, hs);

                auto& bmpobj = icon.bitmapData;
                rndr->d2d1DeviceContext()->DrawBitmap
                (
                /* bitmap               */ bmpobj.data.Get(),
                /* destinationRectangle */ rect,
                /* opacity              */ bmpobj.opacity,
                /* interpolationMode    */ bmpobj.getInterpolationMode()
                );
            }
            else if (std::holds_alternative<Ref<DynamicIcon>>(iconObj))
            {
                auto& icon = std::get<Ref<DynamicIcon>>(iconObj).get();

                auto hs = math_utils::minus(icon.hotSpotOffset);
                auto rect = math_utils::offset(m_absoluteRect, hs);

                icon.bitmapData.draw(rndr, rect);
            }
        }
    }
}
