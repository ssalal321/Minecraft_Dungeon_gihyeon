#include "Player_GetHit.h"
#include "Body_Player.h"

CPlayer_GetHit::CPlayer_GetHit(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEPLAYER_DESC* pDesc)
	: CState_Player(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CPlayer_GetHit::Init_State()
{
	__super::Init_State();

	return S_OK;
}

void CPlayer_GetHit::State_Enter()
{
	// ¹Ù²ã¾ßµÅ!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	m_pActorModelCom->Set_Animation(static_cast<_uint>(PLAYER_STATE::GET_HIT_FRONT), false);
}

void CPlayer_GetHit::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
	
}

void CPlayer_GetHit::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (m_bAnimationFinished)
	{
		if (Change_State_To_Roll())
			return;

		if (Change_State_To_BowAction())
			return;

		if (Change_State_To_Walk())
			return;

		if (Change_State_To_Idle())
			return;
	}
}

void CPlayer_GetHit::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CPlayer_GetHit::State_Exit()
{
}

void CPlayer_GetHit::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);

	//if (pOther->Get_CollidergGroupTag())
}

void CPlayer_GetHit::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CPlayer_GetHit::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Player* CPlayer_GetHit::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEPLAYER_DESC* pDesc)
{
	CPlayer_GetHit* pGameInstance = new CPlayer_GetHit(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CPlayer_GetHit");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CPlayer_GetHit::Free()
{
	__super::Free();
}
