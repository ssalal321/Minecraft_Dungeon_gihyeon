#include "State_Skeleton.h"
#include "Skeleton.h"

#include <random>
#include "Item.h"
#include "Player_Arrow.h"

CState_Skeleton::CState_Skeleton(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATE_SKELETON_DESC* pDesc)
	: CState_Monster(pActor, pGameObjectDesc, pDesc), m_pArrowPool_Monster(pDesc->pArrowPool_Monster)
{
}

HRESULT CState_Skeleton::Init_State()
{
	__super::Init_State();

	if (nullptr == m_pArrowPool_Monster)
		return E_FAIL;

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
	_float lengthToPlayer = m_pSkeleton->Length_To_Player();

	if ((m_pMonsterInfo->fAttackableRange - 1.f < lengthToPlayer) &&
		(lengthToPlayer < m_pMonsterInfo->fAttackableRange + 1.f))
	{
		m_pSkeleton->Change_State(Make_SkeletonState(SKELETON_STATE::BOW_ACTION));
		return true;
	}

	return false;
}


_bool CState_Skeleton::Change_State_To_Walk()
{
	_float lengthToPlayer = m_pSkeleton->Length_To_Player();

	// 플레이어 인지 거리
	// 플레이어가 가까울 땐 좀 떨어지고, 너무 멀면 좀 가까이 간다.
	if (m_pSkeleton->Player_In_DetectRange() &&
		(m_pMonsterInfo->fAttackableRange + 1.f < lengthToPlayer) || 
		(lengthToPlayer < m_pMonsterInfo->fAttackableRange - 1.f))
	{
		m_pSkeleton->Change_State(Make_SkeletonState(SKELETON_STATE::WALK));
		return true;
	}

	return false;
}

_bool CState_Skeleton::Change_State_To_Idle()
{
	_bool	playerInRange = m_pSkeleton->Player_In_DetectRange();

	if (!playerInRange)
	{
		m_pSkeleton->Change_State(Make_SkeletonState(SKELETON_STATE::IDLE));
		return true;
	}

	return false;
}

_bool CState_Skeleton::Change_State_To_HeadSpin()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static uniform_real_distribution<float> dist(0.0f, 1.0f); // 0.0 ~ 1.0 float 확률

	if (dist(gen) < 0.5f) // 50% 확률
	{
		m_pSkeleton->Change_State(Make_SkeletonState(SKELETON_STATE::HEAD_SPIN));
		return true;
	}

	return false;
}

_bool CState_Skeleton::Change_State_To_GetHit(CCollider* pOther)
{
	if (TEXT("Player_Weapon") == pOther->Get_ColliderTag()
		&& pOther->Get_OtherCollisionActivated())
	{
		CItem* pItem = dynamic_cast<CItem*>(pOther->Get_OwnerObject());
		m_pMonsterInfo->Modify_CurrentHp(-pItem->Get_DealPoint());
		m_pSkeleton->Change_State(Make_SkeletonState(SKELETON_STATE::GET_HIT_FRONT));

		return true;
	}

	if (TEXT("Player_Arrow") == pOther->Get_ColliderTag()
		&& pOther->Get_OtherCollisionActivated())
	{
		CPlayer_Arrow* pPlayerArrow = dynamic_cast<CPlayer_Arrow*>(pOther->Get_OwnerObject());
		m_pMonsterInfo->Modify_CurrentHp(-pPlayerArrow->Get_DealPoint());
		m_pSkeleton->Change_State(Make_SkeletonState(SKELETON_STATE::GET_HIT_FRONT));

		return true;
	}

	return false;
}

void CState_Skeleton::Free()
{
	__super::Free();
}
