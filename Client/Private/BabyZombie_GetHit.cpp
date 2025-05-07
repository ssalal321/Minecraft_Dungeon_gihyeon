#include "BabyZombie_GetHit.h"
#include "BabyZombie.h"

CBabyZombie_GetHit::CBabyZombie_GetHit(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_BabyZombie(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CBabyZombie_GetHit::Init_State()
{
	__super::Init_State();

	m_pBabyZombie = dynamic_cast<CBabyZombie*>(m_pActor);
	if (nullptr == m_pBabyZombie)
		return E_FAIL;

	return S_OK;
}

void CBabyZombie_GetHit::State_Enter()
{
	m_pActorModelCom->Set_Animation(static_cast<_uint>(ZOMBIE_STATE::GET_HIT_FRONT), false, 1.0f);
}

void CBabyZombie_GetHit::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CBabyZombie_GetHit::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (m_bAnimationFinished)
	{
		/*if (Change_State_To_Idle())
			return;*/

		if (Change_State_To_Attack())
			return;

		if (Change_State_To_Walk())
			return;
	}
}

void CBabyZombie_GetHit::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CBabyZombie_GetHit::State_Exit()
{
}

void CBabyZombie_GetHit::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);
	
}

void CBabyZombie_GetHit::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CBabyZombie_GetHit::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Monster* CBabyZombie_GetHit::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
{
	CBabyZombie_GetHit* pGameInstance = new CBabyZombie_GetHit(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CBabyZombie_GetHit");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CBabyZombie_GetHit::Free()
{
	__super::Free();
}
