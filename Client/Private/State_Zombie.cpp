#include "State_Zombie.h"

#include "Zombie.h"

CState_Zombie::CState_Zombie(CGameObject* pActor, CModel* pMonsterModelCom, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
                             CTransform* pTransformCom, CNavigation* pNavigationCom)
	: CState_Monster(pActor, pMonsterModelCom, pGameObjectDesc, pTransformCom, pNavigationCom)
{
}

HRESULT CState_Zombie::Init_State()
{
	__super::Init_State();

	return S_OK;
}

void CState_Zombie::State_Enter()
{
}

void CState_Zombie::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);

	// 체력 0이면 죽엇
}

void CState_Zombie::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (m_pZombie->Player_In_Range(TEXT("Prototype_GameObject_PlayerHex"), LEVEL_GAMEPLAY))
	{
		m_pZombie->Change_State(ZOMBIE_STATE::WALK);
	}
}

void CState_Zombie::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CState_Zombie::State_Exit()
{
}

void CState_Zombie::Free()
{
	__super::Free();
}
