#include "Player_Idle.h"
#include "Body_Player.h"

CPlayer_Idle::CPlayer_Idle(CGameObject* pActor, CModel* pPlayerModelCom, CCollider* pColliderCom,
							CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
							CTransform* pTransformCom, CNavigation* pNavigationCom)
	: CState_Player(pActor, pPlayerModelCom, pColliderCom, pGameObjectDesc, pTransformCom, pNavigationCom)
{
}

HRESULT CPlayer_Idle::Init_State()
{
	__super::Init_State();

	return S_OK;
}

void CPlayer_Idle::State_Enter()
{
	m_pActorModelCom->Set_Animation(static_cast<_uint>(PLAYER_STATE::IDLE_GLAIVE), true);
}

void CPlayer_Idle::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
	
}

void CPlayer_Idle::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);
	
}

void CPlayer_Idle::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CPlayer_Idle::State_Exit()
{
}

void CPlayer_Idle::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);
}

void CPlayer_Idle::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CPlayer_Idle::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Player* CPlayer_Idle::Create(CGameObject* pActor, CModel* pPlayerModelCom, CCollider* pColliderCom,
									CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
									CTransform* pTransformCom, CNavigation* pNavigationCom)
{
	CState_Player* pGameInstance = new CPlayer_Idle(pActor, pPlayerModelCom, pColliderCom, pGameObjectDesc, pTransformCom, pNavigationCom);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CPlayer_Idle");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CPlayer_Idle::Free()
{
	__super::Free();
}
