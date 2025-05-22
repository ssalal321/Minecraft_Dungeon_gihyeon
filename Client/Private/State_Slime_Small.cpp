#include "State_Slime_Small.h"
#include "Slime_Small.h"

#include "Player_Arrow.h"
#include "Item.h"


CState_Slime_Small::CState_Slime_Small(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_Monster(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CState_Slime_Small::Init_State()
{
	__super::Init_State();

	return S_OK;
}

void CState_Slime_Small::State_Enter()
{
}

void CState_Slime_Small::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);

	// 체력 0이면 죽엇
}

void CState_Slime_Small::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (m_pMonsterInfo->Get_CurrentHP() <= 0)
	{
		m_pActor->Set_GameObject_Active(false);
		m_pBigColliderCom->Set_ColliderActive(false);
		m_pSmallColliderCom->Set_ColliderActive(false);
	}
}

void CState_Slime_Small::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CState_Slime_Small::State_Exit()
{
}

void CState_Slime_Small::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);
}

void CState_Slime_Small::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CState_Slime_Small::Collision_Exit(CCollider* pOther)
{
}

_bool CState_Slime_Small::Change_State_To_Attack()
{
	// 공격 가능 거리 && 스턴 X 상태
	_float lengthToPlayer = m_pSlime_Small->Length_To_Player();

	if (lengthToPlayer < m_pMonsterInfo->fAttackableRange)
	{
		//m_pNavigationCom->SetUp_On_Navigation(m_pTransformCom);
		m_pTransformCom->Set_Is_Jumping(false);

		m_pSlime_Small->Change_State(Make_Slime_SmallState(SLIME_SMALL_STATE::ATTACK));
		return true;
	}

	return false;
}

_bool CState_Slime_Small::Change_State_To_Walk()
{
	_float lengthToPlayer = m_pSlime_Small->Length_To_Player();

	// 플레이어 인지 거리 && 스턴 X 상태
	if (m_pSlime_Small->Player_In_DetectRange() &&
		lengthToPlayer > m_pMonsterInfo->fAttackableRange)
	{
		m_pSlime_Small->Change_State(Make_Slime_SmallState(SLIME_SMALL_STATE::WALK));
		return true;
	}

	return false;
}

_bool CState_Slime_Small::Change_State_To_Idle()
{
	_bool	playerInRange = m_pSlime_Small->Player_In_DetectRange();

	if (!playerInRange)
	{
		m_pSlime_Small->Change_State(Make_Slime_SmallState(SLIME_SMALL_STATE::IDLE));
		return true;
	}

	return false;
}

_bool CState_Slime_Small::Modify_HP(CCollider* pOther)
{
	if (TEXT("Player_Weapon") == pOther->Get_ColliderTag()
		&& pOther->Get_Other_Collision_Activated())
	{
		CItem* pItem = dynamic_cast<CItem*>(pOther->Get_OwnerObject());
		m_pMonsterInfo->Modify_CurrentHp(-pItem->Get_DealPoint());

		m_pSlime_Small->Render_DamageFont(pItem->Get_DealPoint(), 2.f);

		return true;
	}

	if (TEXT("Player_Arrow") == pOther->Get_ColliderTag()
		&& pOther->Get_Other_Collision_Activated())
	{
		CPlayer_Arrow* pPlayerArrow = dynamic_cast<CPlayer_Arrow*>(pOther->Get_OwnerObject());
		m_pMonsterInfo->Modify_CurrentHp(-pPlayerArrow->Get_DealPoint());

		m_pSlime_Small->Render_DamageFont(pPlayerArrow->Get_DealPoint(), 2.f);

		return true;
	}

	return false;
}

_bool CState_Slime_Small::Change_State_To_Stun(CCollider* pOther)
{
	if (Modify_HP(pOther))
	{
		m_pSlime_Small->Change_State(Make_BabyZombieState(BABYZOMBIE_STATE::GET_HIT));
		return true;
	}

	return false;
}

void CState_Slime_Small::Free()
{
	__super::Free();
}