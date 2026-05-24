#pragma once

#include "Common/Precompile.h"

namespace d14engine::uikit::animation_utils
{
    //------------------------------------------------------------------------
    // Trapezoidal Motion
    //------------------------------------------------------------------------
    // dx --- position in last frame
    // dt --- elapsed time of last frame
    // ds --- total distance
    // t1 --- uniform speed motion time
    // t2 --- variable speed motion time
    // return --- position in next frame
    //------------------------------------------------------------------------
    float advanceTrapezoidalMotion(float dx, float dt, float ds, float t1, float t2);
}
