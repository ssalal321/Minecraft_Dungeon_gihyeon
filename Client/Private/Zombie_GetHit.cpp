#include "Zombie_GetHit.h"
#include "Zombie.h"

CZombie_GetHit::CZombie_GetHit(CGameObject* pActor, CModel* pZombieModelCom, CCollider* pColliderCom,
								CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
								CTransform* pTransformCom, CNavigation* pNavigationCom)
	: CState_Zombie(pActor, pZombieModelCom, pColliderCom, pGameObjectDesc, pTransformCom, pNavigationCom)
{
}

HRESULT CZombie_GetHit::Init_State()
{
	__super::Init_State();

	m_pZombie = dynamic_cast<CZombie*>(m_pActor);

	return S_OK;
}

void CZombie_GetHit::State_Enter()
{
	m_pActorModelCom->Set_Animation(static_cast<_uint>(ZOMBIE_STATE::GET_HIT_FRONT), false);
}

void CZombie_GetHit::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CZombie_GetHit::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (m_bAnimationFinished)
	{
		if (Change_State_To_Idle())
			return;

		if (Change_State_To_Attack())
			return;

		if (Change_State_To_Walk())
			return;
	}
}

void CZombie_GetHit::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CZombie_GetHit::State_Exit()
{
}

void CZombie_GetHit::Collision_Enter(CCollider* pOther)
{
	
}

void CZombie_GetHit::Collision_Stay(CCollider* pOther)
{
	
}

void CZombie_GetHit::Collision_Exit(CCollider* pOther)
{
	
}

CState_Monster* CZombie_GetHit::Create(CGameObject* pActor, CModel* pZombieModelCom, CCollider* pColliderCom,
										CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
										CTransform* pTransformCom, CNavigation* pNavigationCom)
{
	CZombie_GetHit* pGameInstance = new CZombie_GetHit(pActor, pZombieModelCom, pColliderCom, pGameObjectDesc, pTransformCom, pNavigationCom);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CZombie_GetHit");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CZombie_GetHit::Free()
{
	__super::Free();
}
