#include "Zombie_Idle.h"
#include "Zombie.h"

CZombie_Idle::CZombie_Idle(CGameObject* pActor, CModel* pZombieModelCom, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
	CTransform* pTransformCom, CNavigation* pNavigationCom)
	: CState_Monster(pActor, pZombieModelCom, pGameObjectDesc, pTransformCom, pNavigationCom)
{
}

HRESULT CZombie_Idle::Init_State()
{
	__super::Init_State();

	m_pZombie = dynamic_cast<CZombie*>(m_pActor);

	return S_OK;
}

void CZombie_Idle::State_Enter()
{
}

void CZombie_Idle::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CZombie_Idle::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	m_pActorModelCom->Set_Animation(static_cast<_uint>(ZOMBIE_STATE::IDLE), true);
}

void CZombie_Idle::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CZombie_Idle::State_Exit()
{
}

CState_Monster* CZombie_Idle::Create(CGameObject* pActor, CModel* pZombieModelCom, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
							 CTransform* pTransformCom, CNavigation* pNavigationCom)
{
	CZombie_Idle* pGameInstance = new CZombie_Idle(pActor, pZombieModelCom, pGameObjectDesc, pTransformCom, pNavigationCom);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CZombie_Idle");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CZombie_Idle::Free()
{
	__super::Free();
}
