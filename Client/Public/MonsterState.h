#pragma once

#pragma once

#include "Client_Enum.h"

namespace Client
{
	enum class MONSTER_TYPE
	{
		ZOMBIE,
		BABYZOMBIE,
		SKELETON,
		SLIME_LARGE,
		SLIME_MEDIUM,
		SLIME_SMALL,
		SLIME_CAULDRON,
		VINDICATOR,
		TYPE_END
	};

	struct MonsterState
	{
		MONSTER_TYPE monsterType;

		union
		{
			ZOMBIE_STATE		ZombieState;
			BABYZOMBIE_STATE	BabyZombieState;
			SKELETON_STATE		SkeletonState;
			SLIME_LARGE_STATE	SlimeLargeState;
			SLIME_MEDIUM_STATE	SlimeMediumState;
			SLIME_SMALL_STATE	SlimeSmallState;
			SLIME_CAULDRON_STATE	SlimeCauldronState;
			VINDICATOR_STATE	VindicatorState;
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

	inline MonsterState Make_BabyZombieState(BABYZOMBIE_STATE state)
	{
		MonsterState monsterState;
		monsterState.monsterType = MONSTER_TYPE::BABYZOMBIE;
		monsterState.BabyZombieState = state;
		return monsterState;
	}

	// 팩토리 함수: 해골 상태 생성
	inline MonsterState Make_SkeletonState(SKELETON_STATE state)
	{
		MonsterState monsterState;
		monsterState.monsterType = MONSTER_TYPE::SKELETON;
		monsterState.SkeletonState = state;
		return monsterState;
	}

	inline MonsterState Make_Slime_LargeState(SLIME_LARGE_STATE state)
	{
		MonsterState monsterState;
		monsterState.monsterType = MONSTER_TYPE::SLIME_LARGE;
		monsterState.SlimeLargeState = state;
		return monsterState;
	}

	inline MonsterState Make_Slime_MediumState(SLIME_MEDIUM_STATE state)
	{
		MonsterState monsterState;
		monsterState.monsterType = MONSTER_TYPE::SLIME_MEDIUM;
		monsterState.SlimeMediumState = state;
		return monsterState;
	}

	inline MonsterState Make_Slime_SmallState(SLIME_SMALL_STATE state)
	{
		MonsterState monsterState;
		monsterState.monsterType = MONSTER_TYPE::SLIME_SMALL;
		monsterState.SlimeSmallState = state;
		return monsterState;
	}

	inline MonsterState Make_Slime_CauldronState(SLIME_CAULDRON_STATE state)
	{
		MonsterState monsterState;
		monsterState.monsterType = MONSTER_TYPE::SLIME_CAULDRON;
		monsterState.SlimeCauldronState = state;
		return monsterState;
	}

	inline MonsterState Make_VindicatorState(VINDICATOR_STATE state)
	{
		MonsterState monsterState;
		monsterState.monsterType = MONSTER_TYPE::VINDICATOR;
		monsterState.VindicatorState = state;
		return monsterState;
	}
}
