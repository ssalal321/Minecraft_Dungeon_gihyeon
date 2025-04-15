#include "Zombie_Walk.h"

#include "Player.h"
#include "Zombie.h"

CZombie_Walk::CZombie_Walk(CGameObject* pActor, CModel* pZombieModelCom, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
	CTransform* pTransformCom, CNavigation* pNavigationCom)
	: CState_Zombie(pActor, pZombieModelCom, pGameObjectDesc, pTransformCom, pNavigationCom)
{
}

HRESULT CZombie_Walk::Init_State()
{
	__super::Init_State();

	m_pZombie = dynamic_cast<CZombie*>(m_pActor);

	return S_OK;
}

void CZombie_Walk::State_Enter()
{
	m_pActorModelCom->Set_Animation(static_cast<_uint>(ZOMBIE_STATE::WALK), true);
}

void CZombie_Walk::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CZombie_Walk::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (Change_State_To_Idle())
		return;

	if (Change_State_To_Attack())
		return;

	_float4  playerPos = m_pZombie->Get_Player_Position(TEXT("Prototype_GameObject_PlayerHex"), LEVEL_GAMEPLAY);

	m_pTransformCom->LookAt(XMLoadFloat4(&playerPos));

	m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
}

void CZombie_Walk::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CZombie_Walk::State_Exit()
{
}

CState_Monster* CZombie_Walk::Create(CGameObject* pActor, CModel* pZombieModelCom, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
							 CTransform* pTransformCom, CNavigation* pNavigationCom)
{
	CZombie_Walk* pGameInstance = new CZombie_Walk(pActor, pZombieModelCom, pGameObjectDesc, pTransformCom, pNavigationCom);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CZombie_Walk");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CZombie_Walk::Free()
{
	__super::Free();
}
