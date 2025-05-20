#include "CauldronBoss_IntenseSpin.h"

#include <iostream>
#include <ostream>

#include "CauldronBoss.h"

#include "Player.h"
#include "PartObject.h"

CCauldronBoss_IntenseSpin::CCauldronBoss_IntenseSpin(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_CauldronBoss(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CCauldronBoss_IntenseSpin::Init_State()
{
	__super::Init_State();

	m_pCauldronBoss = dynamic_cast<CCauldronBoss*>(m_pActor);
	if (nullptr == m_pCauldronBoss)
		return E_FAIL;

	return S_OK;
}

void CCauldronBoss_IntenseSpin::State_Enter()
{
	m_fSpinTime = 0.f;
	m_pMonsterInfo->Modify_CurrentHp(+20);

	m_pActorModelCom->Set_Animation(static_cast<_uint>(CAULDRONBOSS_STATE::INTENSE_SPIN), false);

	std::cerr << "[CauldronBoss_IntenseSpin]" << std::endl;
}

void CCauldronBoss_IntenseSpin::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CCauldronBoss_IntenseSpin::State_Update(_float fTimeDelta)
{
    __super::State_Update(fTimeDelta);

	if (m_fSpinTime > 2.f)
	{
		if (Change_State_To_Idle())
			return;
	}

	m_fSpinTime += fTimeDelta;
}


void CCauldronBoss_IntenseSpin::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CCauldronBoss_IntenseSpin::State_Exit()
{
}

void CCauldronBoss_IntenseSpin::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);

	Modify_HP(pOther);

	if (TEXT("Monster_Body_Small") == pOther->Get_ColliderTag())
	{
		CMonster* pCurrMonster = dynamic_cast<CMonster*>(dynamic_cast<CPartObject*>(pOther->Get_OwnerObject())->Get_ContainerObject());
		if (false == pCurrMonster->Get_GameObject_Active())
		{
			m_fSpinTime = 0.f;

			m_pMonsterInfo->Modify_CurrentHp(+20);
		}
	}
}

void CCauldronBoss_IntenseSpin::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CCauldronBoss_IntenseSpin::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Monster* CCauldronBoss_IntenseSpin::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
{
	CCauldronBoss_IntenseSpin* pGameInstance = new CCauldronBoss_IntenseSpin(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CCauldronBoss_IntenseSpin");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CCauldronBoss_IntenseSpin::Free()
{
	__super::Free();
}
