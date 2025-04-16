#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum class WINMODE { MODE_FULL, MODE_WIN };

	enum class COLLIDER { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };

	enum class RENDER_MODE { LINE, FILL };

	enum class PROTOTYPE { PROTOTYPE_GAMEOBJECT, PROTOTYPE_COMPONENT };

	enum class MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB };

	enum class MOUSEMOVESTATE {	DIMS_X, DIMS_Y, DIMS_Z };
}
#endif // Engine_Enum_h__
