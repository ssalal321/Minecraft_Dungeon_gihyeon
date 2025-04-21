#pragma once

#pragma once

#include "Client_Enum.h"

namespace Client
{
	enum class MONSTER_TYPE
	{
		ZOMBIE,
		SKELETON,
		TYPE_END
	};

	struct MonsterState
	{
		MONSTER_TYPE monsterType;

		union
		{
			ZOMBIE_STATE	ZombieState;
			SKELETON_STATE	SkeletonState;
		};

		MonsterState() = default;
	};

	// 팩토리 함수: 좀비 상태 생성
	inline MonsterState Make_ZombieState(ZOMBIE_STATE state)
	{
		MonsterState monsterState;
		monsterState.monsterType = MONSTER_TYPE::ZOMBIE;
		monsterState.ZombieState = state;
		return monsterState;
	}

	// 팩토리 함수: 해골 상태 생성
	inline MonsterState Make_SkeletonState(SKELETON_STATE state)
	{
		MonsterState result;
		result.monsterType = MONSTER_TYPE::SKELETON;
		result.SkeletonState = state;
		return result;
	}
}
