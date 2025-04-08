#pragma once

#include "Common/Precompile.h"

#include "Common/MathUtils/2D.h"

#include "UIKit/Appearances/Layout.h"
#include "UIKit/ResizablePanel.h"
#include "UIKit/ResourceUtils.h"

namespace d14engine::uikit
{
    template<typename GeometryInfo_T>
    struct Layout : appearance::Layout, ResizablePanel
    {
        using GeometryInfo = GeometryInfo_T;

        explicit Layout(const D2D1_RECT_F& rect = {})
            :
            Panel(rect, resource_utils::solidColorBrush()),
            ResizablePanel(rect, resource_utils::solidColorBrush())
        {
            setResizable(false);
        }
        _D14_SET_APPEARANCE_PROPERTY(Layout)

        ///////////////////////
        // Interaction Logic //
        ///////////////////////

        //------------------------------------------------------------------
        // Element Geometry Info
        //------------------------------------------------------------------
    protected:
        using ElementGeometryInfoMap = std::unordered_map<SharedPtr<Panel>, GeometryInfo_T>;

        ElementGeometryInfoMap m_elemGeoInfos = {};

        virtual void updateElement(ShrdPtrRefer<Panel> elem, const GeometryInfo_T& geoInfo) = 0;

    public:
        void addElement(ShrdPtrRefer<Panel> elem, const GeometryInfo_T& geoInfo)
        {
            if (elem == nullptr) return;

            addUIObject(elem);
            m_elemGeoInfos.insert({ elem, geoInfo });

            updateElement(elem, geoInfo);
        }

        void removeElement(ShrdPtrRefer<Panel> elem)
        {
            removeUIObject(elem);
            m_elemGeoInfos.erase(elem);
        }

        using ElementGeometryInfoPtr = ElementGeometryInfoMap::iterator;

        Optional<ElementGeometryInfoPtr> findElement(ShrdPtrRefer<Panel> elem)
        {
            auto infoItor = m_elemGeoInfos.find(elem);
            if (infoItor != m_elemGeoInfos.end()) return infoItor;
            else return std::nullopt;
        }

        void clearAllElements()
        {
            clearAddedUIObjects();
            m_elemGeoInfos.clear();
        }

        void updateAllElements()
        {
            for (auto& info : m_elemGeoInfos)
            {
                updateElement(info.first, info.second);
            }
        }

        /////////////////////////
        // Interface Overrides //
        /////////////////////////

    protected:
        //------------------------------------------------------------------
        // IDrawObject2D
        //------------------------------------------------------------------

        void onRendererDrawD2d1ObjectHelper(Renderer* rndr) override
        {
            ////////////////
            // Background //
            ////////////////

            auto& background = appearance().background;

            resource_utils::solidColorBrush()->SetColor(background.color);
            resource_utils::solidColorBrush()->SetOpacity(background.opacity);

            Panel::drawBackground(rndr);
        }

        void drawD2d1ObjectPosterior(Renderer* rndr) override
        {
            Panel::drawD2d1ObjectPosterior(rndr);

            /////////////
            // Outline //
            /////////////

            auto& stroke = appearance().stroke;

            resource_utils::solidColorBrush()->SetColor(stroke.color);
            resource_utils::solidColorBrush()->SetOpacity(stroke.opacity);

            auto rect = math_utils::inner(m_absoluteRect, stroke.width);
            D2D1_ROUNDED_RECT roundedRect = { rect, roundRadiusX, roundRadiusY };

            rndr->d2d1DeviceContext()->DrawRoundedRectangle
            (
            /* roundedRect */ roundedRect,
            /* brush       */ resource_utils::solidColorBrush(),
            /* strokeWidth */ stroke.width
            );
        }
        //------------------------------------------------------------------
        // Panel
        //------------------------------------------------------------------

        bool releaseUIObjectHelper(ShrdPtrRefer<Panel> uiobj) override
        {
            removeElement(uiobj); return true;
        }

        void onSizeHelper(SizeEvent& e) override
        {
            ResizablePanel::onSizeHelper(e);

            updateAllElements();
        }

        void onChangeThemeStyleHelper(const ThemeStyle& style) override
        {
            ResizablePanel::onChangeThemeStyleHelper(style);

            appearance().changeTheme(style.name);
        }
    };
}
