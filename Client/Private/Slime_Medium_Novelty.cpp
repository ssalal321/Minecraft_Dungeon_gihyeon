#include "BabyZombie_Novelty.h"
#include "BabyZombie.h"

CBabyZombie_Novelty::CBabyZombie_Novelty(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_BabyZombie(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CBabyZombie_Novelty::Init_State()
{
	__super::Init_State();

	m_pBabyZombie = dynamic_cast<CBabyZombie*>(m_pActor);
	if (nullptr == m_pBabyZombie)
		return E_FAIL;

	return S_OK;
}

void CBabyZombie_Novelty::State_Enter()
{
	m_pActorModelCom->Set_Animation(static_cast<_uint>(BABYZOMBIE_STATE::NOVELTY), false);
}

void CBabyZombie_Novelty::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CBabyZombie_Novelty::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (Change_State_To_Attack())
		return;

	if (Change_State_To_Walk())
		return;

	if (m_bAnimationFinished)
	{
		m_pBabyZombie->Change_State(Make_BabyZombieState(BABYZOMBIE_STATE::IDLE));
	}
}

void CBabyZombie_Novelty::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CBabyZombie_Novelty::State_Exit()
{
}

void CBabyZombie_Novelty::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);

	Change_State_To_GetHit(pOther);
}

void CBabyZombie_Novelty::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CBabyZombie_Novelty::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Monster* CBabyZombie_Novelty::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
{
	CBabyZombie_Novelty* pGameInstance = new CBabyZombie_Novelty(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CBabyZombie_Novelty");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CBabyZombie_Novelty::Free()
{
	__super::Free();
}
