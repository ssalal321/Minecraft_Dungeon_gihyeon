#include "Zombie_Stun.h"
#include "Zombie.h"

CZombie_Stun::CZombie_Stun(CGameObject* pActor, CModel* pZombieModelCom, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
	CTransform* pTransformCom, CNavigation* pNavigationCom)
	: CState_Zombie(pActor, pZombieModelCom, pGameObjectDesc, pTransformCom, pNavigationCom)
{
}

HRESULT CZombie_Stun::Init_State()
{
	__super::Init_State();

	m_pZombie = dynamic_cast<CZombie*>(m_pActor);

	return S_OK;
}

void CZombie_Stun::State_Enter()
{
	m_pActorModelCom->Set_Animation(static_cast<_uint>(ZOMBIE_STATE::STUN), false);
}

void CZombie_Stun::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CZombie_Stun::State_Update(_float fTimeDelta)
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

void CZombie_Stun::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CZombie_Stun::State_Exit()
{
}

CState_Monster* CZombie_Stun::Create(CGameObject* pActor, CModel* pZombieModelCom, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
							 CTransform* pTransformCom, CNavigation* pNavigationCom)
{
	CZombie_Stun* pGameInstance = new CZombie_Stun(pActor, pZombieModelCom, pGameObjectDesc, pTransformCom, pNavigationCom);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CZombie_Stun");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CZombie_Stun::Free()
{
	__super::Free();
}
