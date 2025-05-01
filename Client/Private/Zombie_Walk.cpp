#include "Zombie_Walk.h"

#include "Player.h"
#include "Zombie.h"

CZombie_Walk::CZombie_Walk(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_Zombie(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CZombie_Walk::Init_State()
{
	__super::Init_State();

	m_pZombie = dynamic_cast<CZombie*>(m_pActor);
	if (nullptr == m_pZombie)
		return E_FAIL;

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

	_float4  playerPos = m_pZombie->Get_Player_Position(TEXT("GameObject_Player"), LEVEL_GAMEPLAY);

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

void CZombie_Walk::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);

	Change_State_To_GetHit();
}

void CZombie_Walk::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CZombie_Walk::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Monster* CZombie_Walk::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
{
	CZombie_Walk* pGameInstance = new CZombie_Walk(pActor, pGameObjectDesc, pDesc);

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
