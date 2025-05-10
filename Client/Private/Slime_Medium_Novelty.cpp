#include "Slime_Medium_Novelty.h"
#include "Slime_Medium.h"

CSlime_Medium_Novelty::CSlime_Medium_Novelty(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_Slime_Medium(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CSlime_Medium_Novelty::Init_State()
{
	__super::Init_State();

	m_pSlime_Medium = dynamic_cast<CSlime_Medium*>(m_pActor);
	if (nullptr == m_pSlime_Medium)
		return E_FAIL;

	return S_OK;
}

void CSlime_Medium_Novelty::State_Enter()
{
	m_pActorModelCom->Set_Animation(static_cast<_uint>(SLIME_MEDIUM_STATE::NOVELTY), false);
}

void CSlime_Medium_Novelty::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CSlime_Medium_Novelty::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (Change_State_To_Attack())
		return;

	if (Change_State_To_Walk())
		return;

	if (m_bAnimationFinished)
	{
		m_pSlime_Medium->Change_State(Make_Slime_MediumState(SLIME_MEDIUM_STATE::IDLE));
	}
}

void CSlime_Medium_Novelty::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CSlime_Medium_Novelty::State_Exit()
{
}

void CSlime_Medium_Novelty::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);

	Change_State_To_Stun(pOther);
}

void CSlime_Medium_Novelty::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CSlime_Medium_Novelty::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Monster* CSlime_Medium_Novelty::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
{
	CSlime_Medium_Novelty* pGameInstance = new CSlime_Medium_Novelty(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CSlime_Medium_Novelty");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CSlime_Medium_Novelty::Free()
{
	__super::Free();
}
