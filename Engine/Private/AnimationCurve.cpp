#include "AnimationCurve.h"
#include <cmath>  // std::pow 함수 사용을 위한 헤더

BEGIN (Engine)
// EaseOutBack 함수 구현
float CAnimationCurve::EaseOutBack(float t)
{
    const float c1 = 1.70158f;
    const float c3 = c1 + 1.f;
    return 1.f + c3 * std::pow(t - 1.f, 3.f) + c1 * std::pow(t - 1.f, 2.f);
}

END;