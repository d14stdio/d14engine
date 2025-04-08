#pragma once

#include "Common/Precompile.h"

#include "UIKit/Layout.h"

namespace d14engine::uikit
{
    struct ConstraintLayoutGeometryInfo
    {
        bool keepWidth = true;

        struct HorzDistance
        {
            Optional<float> ToLeft = {}, ToRight = {};
        }
        Left = {}, Right = {};

        bool keepHeight = true;

        struct VertDistance
        {
            Optional<float> ToTop = {}, ToBottom = {};
        }
        Top = {}, Bottom = {};
    };
    struct TreeViewItem;

    template<typename Item_T>
    struct WaterfallView;

    struct ConstraintLayout : Layout<ConstraintLayoutGeometryInfo>
    {
        friend TreeViewItem;

        // Due to the syntax issue of friend template,
        // the keyword (struct) here cannot be omitted.
        template<typename Item_T>
        friend struct WaterfallView;

        explicit ConstraintLayout(const D2D1_RECT_F& rect = {});

        ///////////////////////
        // Interaction Logic //
        ///////////////////////

        //------------------------------------------------------------------
        // Element Geometry Info
        //------------------------------------------------------------------
    protected:
        void updateElement(ShrdPtrRefer<Panel> elem, const GeometryInfo& geoInfo) override;
    };
}
