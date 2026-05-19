#include "Common/Precompile.h"

#include "UIKit/FrameAnimPanel.h"

using namespace d14engine::renderer;

namespace d14engine::uikit
{
    void FrameAnimPanel::onRendererUpdateObject2DHelper(Renderer* rndr)
    {
        if (m_isAnimating) bitmapData.update(rndr);
    }

    void FrameAnimPanel::onRendererDrawD2d1ObjectHelper(Renderer* rndr)
    {
        bitmapData.draw(rndr, m_absoluteRect);
    }
}
