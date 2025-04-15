#include "Zombie_Attack.h"
#include "Zombie.h"

CZombie_Attack::CZombie_Attack(CGameObject* pActor, CModel* pZombieModelCom, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
	CTransform* pTransformCom, CNavigation* pNavigationCom)
	: CState_Zombie(pActor, pZombieModelCom, pGameObjectDesc, pTransformCom, pNavigationCom)
{
}

HRESULT CZombie_Attack::Init_State()
{
	__super::Init_State();

	m_pZombie = dynamic_cast<CZombie*>(m_pActor);

	return S_OK;
}

void CZombie_Attack::State_Enter()
{
	m_pActorModelCom->Set_Animation(static_cast<_uint>(ZOMBIE_STATE::ATTACK), true);
}

void CZombie_Attack::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CZombie_Attack::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	_float4  playerPos = m_pZombie->Get_Player_Position(TEXT("Prototype_GameObject_PlayerHex"), LEVEL_GAMEPLAY);

	m_pTransformCom->LookAt(XMLoadFloat4(&playerPos));

	/*_float lengthToPlayer = m_pZombie->Length_To_Player(TEXT("Prototype_GameObject_PlayerHex"), LEVEL_GAMEPLAY);

	if (lengthToPlayer > m_pMonsterDesc->fAttackableRange)
	{
		m_pZombie->Change_State(ZOMBIE_STATE::WALK);
		return;
	}*/
}

void CZombie_Attack::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CZombie_Attack::State_Exit()
{
}

CState_Monster* CZombie_Attack::Create(CGameObject* pActor, CModel* pZombieModelCom, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
							 CTransform* pTransformCom, CNavigation* pNavigationCom)
{
	CZombie_Attack* pGameInstance = new CZombie_Attack(pActor, pZombieModelCom, pGameObjectDesc, pTransformCom, pNavigationCom);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CZombie_Attack");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CZombie_Attack::Free()
{
	__super::Free();
}
