#pragma once
#include "Engine_Macro.h"

BEGIN (Engine)
class ENGINE_DLL CAnimationCurve final
{
public:
	// EaseOutBack 함수 선언
	static float EaseOutBack(float t);
};

END;