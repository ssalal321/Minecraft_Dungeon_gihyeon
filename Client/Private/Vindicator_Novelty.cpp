#include "Vindicator_Novelty.h"
#include "Vindicator.h"

CVindicator_Novelty::CVindicator_Novelty(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_Vindicator(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CVindicator_Novelty::Init_State()
{
	__super::Init_State();

	m_pVindicator = dynamic_cast<CVindicator*>(m_pActor);
	if (nullptr == m_pVindicator)
		return E_FAIL;

	return S_OK;
}

void CVindicator_Novelty::State_Enter()
{
	m_pActorModelCom->Set_Animation(static_cast<_uint>(VINDICATOR_STATE::NOVELTY), false);
}

void CVindicator_Novelty::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CVindicator_Novelty::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (Change_State_To_Attack())
		return;

	if (Change_State_To_Walk())
		return;

	if (m_bAnimationFinished)
	{
		m_pVindicator->Change_State(Make_VindicatorState(VINDICATOR_STATE::IDLE));
	}
}

void CVindicator_Novelty::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CVindicator_Novelty::State_Exit()
{
}

void CVindicator_Novelty::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);

	Modify_HP(pOther);
}

void CVindicator_Novelty::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CVindicator_Novelty::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Monster* CVindicator_Novelty::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
{
	CVindicator_Novelty* pGameInstance = new CVindicator_Novelty(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CVindicator_Novelty");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CVindicator_Novelty::Free()
{
	__super::Free();
}
