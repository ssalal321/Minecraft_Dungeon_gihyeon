#include "CauldronBoss_Idle.h"
#include "State_CauldronBoss.h"
#include "CauldronBoss.h"

#include <random>

CCauldronBoss_Idle::CCauldronBoss_Idle(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_CauldronBoss(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CCauldronBoss_Idle::Init_State()
{
	__super::Init_State();

	m_pCauldronBoss = dynamic_cast<CCauldronBoss*>(m_pActor);
	if (nullptr == m_pCauldronBoss)
		return E_FAIL;

	return S_OK;
}

void CCauldronBoss_Idle::State_Enter()
{
	m_pActorModelCom->Set_Animation(static_cast<_uint>(CAULDRONBOSS_STATE::IDLE), false);
}

void CCauldronBoss_Idle::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
	
}

void CCauldronBoss_Idle::State_Update(_float fTimeDelta)
{
	// basicAttack과 StrongAttack 번갈아 나오게 할까..

	__super::State_Update(fTimeDelta);

	if (m_bAnimationFinished)
	{
		if (m_bNextIsBasicAttack)
		{
			if (Change_State_To_BasicAttack())
			{
				m_bNextIsBasicAttack = false;
				return;
			}
		}
		else
		{
			if (Change_State_To_StrongAttack())
			{
				m_bNextIsBasicAttack = true;
				return;
			}
		}
	}
}

void CCauldronBoss_Idle::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CCauldronBoss_Idle::State_Exit()
{
}

void CCauldronBoss_Idle::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);

	Modify_HP(pOther);

	if (Change_State_To_IntenseSpin(pOther))
		return;
}

void CCauldronBoss_Idle::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CCauldronBoss_Idle::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Monster* CCauldronBoss_Idle::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
{
	CCauldronBoss_Idle* pGameInstance = new CCauldronBoss_Idle(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CCauldronBoss_Idle");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CCauldronBoss_Idle::Free()
{
	__super::Free();
}
