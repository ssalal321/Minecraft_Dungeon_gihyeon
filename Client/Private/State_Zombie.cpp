#include "State_Zombie.h"

#include <iostream>

#include "Zombie.h"

#include "Player_Arrow.h"
#include "Item.h"


CState_Zombie::CState_Zombie(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_Monster(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CState_Zombie::Init_State()
{
	__super::Init_State();

	return S_OK;
}

void CState_Zombie::State_Enter()
{
}

void CState_Zombie::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);

	// 체력 0이면 죽엇
}

void CState_Zombie::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	// 공격 받았을 때 스턴 걸리기
}

void CState_Zombie::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CState_Zombie::State_Exit()
{
}

void CState_Zombie::Collision_Enter(CCollider* pOther)
{
//#ifdef DEBUG
//	_wstring other = pOther->Get_ColliderTag();
//	std::wcerr << "[좀비와 " << other << " 충돌 Enter]" << std::endl;
//#endif
}

void CState_Zombie::Collision_Stay(CCollider* pOther)
{
	/*_wstring other = pOther->Get_CollidergGroupTag();

	std::wcerr << "[좀비와 " << other << " 충돌 Stay]" << std::endl;*/
}

void CState_Zombie::Collision_Exit(CCollider* pOther)
{
	/*_wstring other = pOther->Get_CollidergGroupTag();

	std::wcerr << "[좀비와 " << other << " 충돌 Exit]" << std::endl;*/
}

_bool CState_Zombie::Change_State_To_Attack()
{
	// 공격 가능 거리 && 스턴 X 상태
	_float lengthToPlayer = m_pZombie->Length_To_Player();

	if (lengthToPlayer < m_pMonsterInfo->fAttackableRange)
	{
		m_pZombie->Change_State(Make_ZombieState(ZOMBIE_STATE::ATTACK));
		return true;
	}

	return false;
}

_bool CState_Zombie::Change_State_To_Walk()
{
	_float lengthToPlayer = m_pZombie->Length_To_Player();

	// 플레이어 인지 거리 && 스턴 X 상태
	if (m_pZombie->Player_In_DetectRange() &&
		lengthToPlayer > m_pMonsterInfo->fAttackableRange)
	{
		m_pZombie->Change_State(Make_ZombieState(ZOMBIE_STATE::WALK));
		return true;
	}

	return false;
}

_bool CState_Zombie::Change_State_To_Idle()
{
	_bool	playerInRange = m_pZombie->Player_In_DetectRange();

	if (!playerInRange)
	{
		m_pZombie->Change_State(Make_ZombieState(ZOMBIE_STATE::IDLE));
		return true;
	}

	return false;
}

_bool CState_Zombie::Change_State_To_GetHit(CCollider* pOther)
{
	if (TEXT("Player_Weapon") == pOther->Get_ColliderTag()
		&& pOther->Get_OtherCollisionActivated())
	{
		CItem* pItem = dynamic_cast<CItem*>(pOther->Get_OwnerObject());
		m_pMonsterInfo->Modify_CurrentHp(-pItem->Get_DealPoint());
		m_pZombie->Change_State(Make_SkeletonState(SKELETON_STATE::GET_HIT_FRONT));

		return true;
	}

	if (TEXT("Player_Arrow") == pOther->Get_ColliderTag()
		&& pOther->Get_OtherCollisionActivated())
	{
		CPlayer_Arrow* pPlayerArrow = dynamic_cast<CPlayer_Arrow*>(pOther->Get_OwnerObject());
		m_pMonsterInfo->Modify_CurrentHp(-pPlayerArrow->Get_DealPoint());
		m_pZombie->Change_State(Make_SkeletonState(SKELETON_STATE::GET_HIT_FRONT));

		return true;
	}

	return false;
}

//void CState_Zombie::Modify_HP(CCollider* pOther)
//{
//	if (TEXT("Player_Weapon") == pOther->Get_ColliderTag())
//	{
//		CItem* pItem = dynamic_cast<CItem*>(pOther->Get_OwnerObject());
//		m_pMonsterInfo->Modify_CurrentHp(-pItem->Get_DealPoint());
//	}
//
//	if (TEXT("Player_Arrow") == pOther->Get_ColliderTag())
//	{
//		CPlayer_Arrow* pPlayerArrow = dynamic_cast<CPlayer_Arrow*>(pOther->Get_OwnerObject());
//		m_pMonsterInfo->Modify_CurrentHp(-pPlayerArrow->Get_DealPoint());
//	}
//}

void CState_Zombie::Free()
{
	__super::Free();
}