#include "State_BabyZombie.h"
#include "BabyZombie.h"

#include "Player_Arrow.h"
#include "Item.h"


CState_BabyZombie::CState_BabyZombie(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_Monster(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CState_BabyZombie::Init_State()
{
	__super::Init_State();

	return S_OK;
}

void CState_BabyZombie::State_Enter()
{
}

void CState_BabyZombie::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CState_BabyZombie::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (Change_State_To_Dead())
		return;
}

void CState_BabyZombie::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CState_BabyZombie::State_Exit()
{
}

void CState_BabyZombie::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);

	
}

void CState_BabyZombie::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CState_BabyZombie::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

_bool CState_BabyZombie::Change_State_To_Attack()
{
	// 공격 가능 거리 && 스턴 X 상태
	_float lengthToPlayer = m_pBabyZombie->Length_To_Player();

	if (lengthToPlayer < m_pMonsterInfo->fAttackableRange)
	{
		m_pBabyZombie->Change_State(Make_BabyZombieState(BABYZOMBIE_STATE::ATTACK));
		return true;
	}

	return false;
}

_bool CState_BabyZombie::Change_State_To_Walk()
{
	_float lengthToPlayer = m_pBabyZombie->Length_To_Player();

	// 플레이어 인지 거리
	if (m_pBabyZombie->Player_In_DetectRange()/* &&
		lengthToPlayer > m_pMonsterInfo->fAttackableRange*/)
	{
		m_pBabyZombie->Change_State(Make_BabyZombieState(BABYZOMBIE_STATE::WALK));
		return true;
	}

	return false;
}

_bool CState_BabyZombie::Change_State_To_Idle()
{
	_bool	playerInRange = m_pBabyZombie->Player_In_DetectRange();

	if (!playerInRange)
	{
		m_pBabyZombie->Change_State(Make_BabyZombieState(BABYZOMBIE_STATE::IDLE));
		return true;
	}

	return false;
}

_bool CState_BabyZombie::Change_State_To_Dead()
{
	if (m_pMonsterInfo->Get_CurrentHP() <= 0)
	{
		m_pBabyZombie->Change_State(Make_BabyZombieState(BABYZOMBIE_STATE::DEAD));
		return true;
	}
	
	return false;
}

_bool CState_BabyZombie::Modify_HP(CCollider* pOther)
{
	if (TEXT("Player_Weapon") == pOther->Get_ColliderTag()
		&& pOther->Get_Other_Collision_Activated())
	{
		CItem* pItem = dynamic_cast<CItem*>(pOther->Get_OwnerObject());
		m_pMonsterInfo->Modify_CurrentHp(-pItem->Get_DealPoint());

		return true;
	}

	if (TEXT("Player_Arrow") == pOther->Get_ColliderTag()
		&& pOther->Get_Other_Collision_Activated())
	{
		CPlayer_Arrow* pPlayerArrow = dynamic_cast<CPlayer_Arrow*>(pOther->Get_OwnerObject());
		m_pMonsterInfo->Modify_CurrentHp(-pPlayerArrow->Get_DealPoint());

		return true;
	}

	return false;
}

_bool CState_BabyZombie::Change_State_To_GetHit(CCollider* pOther)
{
	if (Modify_HP(pOther))
	{
		m_pBabyZombie->Change_State(Make_BabyZombieState(BABYZOMBIE_STATE::GET_HIT));
		return true;
	}

	return false;
}

void CState_BabyZombie::Free()
{
	__super::Free();
}