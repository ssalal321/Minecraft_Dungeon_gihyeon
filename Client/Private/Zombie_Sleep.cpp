#include "Zombie_Sleep.h"
#include "Zombie.h"

CZombie_Sleep::CZombie_Sleep(CGameObject* pActor, CModel* pZombieModelCom, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
	CTransform* pTransformCom, CNavigation* pNavigationCom)
	: CState_Zombie(pActor, pZombieModelCom, pGameObjectDesc, pTransformCom, pNavigationCom)
{
}

HRESULT CZombie_Sleep::Init_State()
{
	__super::Init_State();

	m_pZombie = dynamic_cast<CZombie*>(m_pActor);

	return S_OK;
}

void CZombie_Sleep::State_Enter()
{
	m_pActorModelCom->Set_Animation(static_cast<_uint>(ZOMBIE_STATE::NOVELTY_SLEEP), false);
}

void CZombie_Sleep::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CZombie_Sleep::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (m_bAnimationFinished)
	{
		m_pZombie->Change_State(ZOMBIE_STATE::IDLE);
		return;
	}

}

void CZombie_Sleep::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CZombie_Sleep::State_Exit()
{
}

CState_Monster* CZombie_Sleep::Create(CGameObject* pActor, CModel* pZombieModelCom, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
							 CTransform* pTransformCom, CNavigation* pNavigationCom)
{
	CZombie_Sleep* pGameInstance = new CZombie_Sleep(pActor, pZombieModelCom, pGameObjectDesc, pTransformCom, pNavigationCom);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CZombie_Sleep");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CZombie_Sleep::Free()
{
	__super::Free();
}
