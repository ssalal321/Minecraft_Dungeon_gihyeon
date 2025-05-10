#include "State_Slime_Medium.h"
#include "Slime_Medium.h"

#include "Player_Arrow.h"
#include "Item.h"


CState_Slime_Medium::CState_Slime_Medium(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_Monster(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CState_Slime_Medium::Init_State()
{
	__super::Init_State();

	return S_OK;
}

void CState_Slime_Medium::State_Enter()
{
}

void CState_Slime_Medium::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);

	// 체력 0이면 죽엇
}

void CState_Slime_Medium::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	// 공격 받았을 때 스턴 걸리기
}

void CState_Slime_Medium::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CState_Slime_Medium::State_Exit()
{
}

void CState_Slime_Medium::Collision_Enter(CCollider* pOther)
{
}

void CState_Slime_Medium::Collision_Stay(CCollider* pOther)
{
}

void CState_Slime_Medium::Collision_Exit(CCollider* pOther)
{
}

_bool CState_Slime_Medium::Change_State_To_Attack()
{
	// 공격 가능 거리 && 스턴 X 상태
	_float lengthToPlayer = m_pSlime_Medium->Length_To_Player();

	if (lengthToPlayer < m_pMonsterInfo->fAttackableRange)
	{
		m_pNavigationCom->SetUp_On_Navigation(m_pTransformCom);
		m_pTransformCom->Set_Is_Jumping(false);

		m_pSlime_Medium->Change_State(Make_Slime_MediumState(SLIME_MEDIUM_STATE::ATTACK));
		return true;
	}

	return false;
}

_bool CState_Slime_Medium::Change_State_To_Walk()
{
	_float lengthToPlayer = m_pSlime_Medium->Length_To_Player();

	// 플레이어 인지 거리 && 스턴 X 상태
	if (m_pSlime_Medium->Player_In_DetectRange() &&
		lengthToPlayer > m_pMonsterInfo->fAttackableRange)
	{
		m_pSlime_Medium->Change_State(Make_Slime_MediumState(SLIME_MEDIUM_STATE::WALK));
		return true;
	}

	return false;
}

_bool CState_Slime_Medium::Change_State_To_Idle()
{
	_bool	playerInRange = m_pSlime_Medium->Player_In_DetectRange();

	if (!playerInRange)
	{
		m_pSlime_Medium->Change_State(Make_Slime_MediumState(SLIME_MEDIUM_STATE::IDLE));
		return true;
	}

	return false;
}

_bool CState_Slime_Medium::Change_State_To_Stun(CCollider* pOther)
{
	if (TEXT("Player_Weapon") == pOther->Get_ColliderTag()
		&& pOther->Get_Other_Collision_Activated())
	{
		CItem* pItem = dynamic_cast<CItem*>(pOther->Get_OwnerObject());
		m_pMonsterInfo->Modify_CurrentHp(-pItem->Get_DealPoint());
		m_pSlime_Medium->Change_State(Make_Slime_MediumState(SLIME_MEDIUM_STATE::STUN));

		return true;
	}

	if (TEXT("Player_Arrow") == pOther->Get_ColliderTag()
		&& pOther->Get_Other_Collision_Activated())
	{
		CPlayer_Arrow* pPlayerArrow = dynamic_cast<CPlayer_Arrow*>(pOther->Get_OwnerObject());
		m_pMonsterInfo->Modify_CurrentHp(-pPlayerArrow->Get_DealPoint());
		m_pSlime_Medium->Change_State(Make_Slime_MediumState(SLIME_MEDIUM_STATE::STUN));

		return true;
	}

	return false;
}

void CState_Slime_Medium::Free()
{
	__super::Free();
}