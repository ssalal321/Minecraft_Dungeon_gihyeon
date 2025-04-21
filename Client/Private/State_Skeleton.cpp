#include "State_Skeleton.h"

#include <iostream>

#include "Skeleton.h"

CState_Skeleton::CState_Skeleton(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_Monster(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CState_Skeleton::Init_State()
{
	__super::Init_State();

	return S_OK;
}

void CState_Skeleton::State_Enter()
{
}

void CState_Skeleton::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);

	// 체력 0이면 죽엇
}

void CState_Skeleton::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	// 공격 받았을 때 스턴 걸리기
}

void CState_Skeleton::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CState_Skeleton::State_Exit()
{
}

void CState_Skeleton::Collision_Enter(CCollider* pOther)
{
	if (Change_State_To_GetHit(pOther))
		return;
}

void CState_Skeleton::Collision_Stay(CCollider* pOther)
{
}

void CState_Skeleton::Collision_Exit(CCollider* pOther)
{
}


_bool CState_Skeleton::Change_State_To_Attack()  // 얘도 Walk 내부에서만 호출해도 괜찮을지도
{
	// 공격 가능 거리
	// 너무 멀지도, 너무 가깝지도 않을 때.
	_uint  currentLevelIndex = m_pGameInstance->Get_CurrentLevelIndex();

	_float lengthToPlayer = m_pSkeleton->Length_To_Player(TEXT("Prototype_GameObject_PlayerHex"), currentLevelIndex);

	if ((m_pMonsterDesc->fAttackableRange - 1.f < lengthToPlayer) &&
		(lengthToPlayer < m_pMonsterDesc->fAttackableRange + 1.f))
	{
		m_pSkeleton->Change_State(Make_SkeletonState(SKELETON_STATE::BOW_ACTION));
		return true;
	}

	return false;
}


_bool CState_Skeleton::Change_State_To_Walk()
{
	_uint  currentLevelIndex = m_pGameInstance->Get_CurrentLevelIndex();
	_float lengthToPlayer = m_pSkeleton->Length_To_Player(TEXT("Prototype_GameObject_PlayerHex"), currentLevelIndex);

	// 플레이어 인지 거리
	// 플레이어가 가까울 땐 좀 떨어지고, 너무 멀면 좀 가까이 간다.
	if (m_pSkeleton->Player_In_DetectRange(TEXT("Prototype_GameObject_PlayerHex"), currentLevelIndex) &&
		(m_pMonsterDesc->fAttackableRange + 1.f < lengthToPlayer) || 
		(lengthToPlayer < m_pMonsterDesc->fAttackableRange - 1.f))
	{
		m_pSkeleton->Change_State(Make_SkeletonState(SKELETON_STATE::WALK));
		return true;
	}

	return false;
}

_bool CState_Skeleton::Change_State_To_Idle()
{
	_uint  currentLevelIndex = m_pGameInstance->Get_CurrentLevelIndex();

	_bool	playerInRange = m_pSkeleton->Player_In_DetectRange(TEXT("Prototype_GameObject_PlayerHex"), currentLevelIndex);

	if (!playerInRange)
	{
		m_pSkeleton->Change_State(Make_SkeletonState(SKELETON_STATE::IDLE));
		return true;
	}

	return false;
}

_bool CState_Skeleton::Change_State_To_GetHit(CCollider* pOther)
{
	if (TEXT("PlayerWeapon_OBB") == pOther->Get_OwnerTag() && pOther->Get_OtherAttacking())
	{
		m_pSkeleton->Change_State(Make_SkeletonState(SKELETON_STATE::GET_HIT_FRONT));

		return true;
	}

	return false;
}

void CState_Skeleton::Free()
{
	__super::Free();
}
