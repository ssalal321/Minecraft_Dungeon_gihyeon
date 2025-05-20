#include "State_Slime_Cauldron.h"

#include <iostream>

#include "Player_Arrow.h"
#include "Item.h"

CState_Slime_Cauldron::CState_Slime_Cauldron(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATE_SLIME_CAULDRON_DESC* pDesc)
	: CState_Monster(pActor, pGameObjectDesc, pDesc), m_pBulletPool_Monster(pDesc->pBulletPool_Monster)
{
}

HRESULT CState_Slime_Cauldron::Init_State()
{
	__super::Init_State();

	return S_OK;
}

void CState_Slime_Cauldron::State_Enter()
{
}

void CState_Slime_Cauldron::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);

	// 체력 0이면 죽엇
}

void CState_Slime_Cauldron::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (m_pMonsterInfo->Get_CurrentHP() <= 0)
	{
		m_pActor->Set_GameObject_Active(false);
		m_pBigColliderCom->Set_ColliderActive(false);
		m_pSmallColliderCom->Set_ColliderActive(false);
	}

	m_pTransformCom->Jump(fTimeDelta, m_pNavigationCom);
	if (m_pTransformCom->Get_Is_Jumping())
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
}

void CState_Slime_Cauldron::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CState_Slime_Cauldron::State_Exit()
{
}

void CState_Slime_Cauldron::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);
}

void CState_Slime_Cauldron::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CState_Slime_Cauldron::Collision_Exit(CCollider* pOther)
{
}

_bool CState_Slime_Cauldron::Modify_HP(CCollider* pOther)
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

void CState_Slime_Cauldron::Free()
{
	__super::Free();
}