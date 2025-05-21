#include "CauldronBoss_TPose.h"

#include <iostream>

#include "State_CauldronBoss.h"
#include "CauldronBoss.h"
#include "Body_CauldronBoss.h"


CCauldronBoss_TPose::CCauldronBoss_TPose(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_CauldronBoss(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CCauldronBoss_TPose::Init_State()
{
	__super::Init_State();

	m_pCauldronBoss = dynamic_cast<CCauldronBoss*>(m_pActor);
	if (nullptr == m_pCauldronBoss)
		return E_FAIL;

	m_pBody_CauldronBoss = dynamic_cast<CBody_CauldronBoss*>(m_pCauldronBoss->Find_PartObject(TEXT("Part_Body")));

	m_pTransformCom->Rotation({ 0.f, 1.f, 0.f }, 135.1f);

	return S_OK;
}

void CCauldronBoss_TPose::State_Enter()
{
	m_pActorModelCom->Set_Animation(static_cast<_uint>(CAULDRONBOSS_STATE::TPOSE), true);

	std::cerr << "[CauldronBoss_TPose]" << std::endl;
}

void CCauldronBoss_TPose::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CCauldronBoss_TPose::State_Update(_float fTimeDelta)
{
	if (false == m_pBody_CauldronBoss->Get_Appearing())
	{
		Change_State_To_Idle();
		return;
	}

	__super::State_Update(fTimeDelta);
}

void CCauldronBoss_TPose::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CCauldronBoss_TPose::State_Exit()
{
}

void CCauldronBoss_TPose::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);
}

void CCauldronBoss_TPose::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CCauldronBoss_TPose::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);

	m_pTransformCom->Rotation({ 0.f, 1.f, 0.f }, -135.1f);
}

CState_Monster* CCauldronBoss_TPose::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
{
	CCauldronBoss_TPose* pGameInstance = new CCauldronBoss_TPose(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CCauldronBoss_TPose");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CCauldronBoss_TPose::Free()
{
	__super::Free();
}
