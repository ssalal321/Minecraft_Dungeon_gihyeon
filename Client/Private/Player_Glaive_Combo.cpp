#include "Player_Glaive_Combo.h"
#include "Body_Player.h"

CPlayer_Glaive_Combo::CPlayer_Glaive_Combo(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEPLAYER_DESC* pDesc)
	: CState_Player(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CPlayer_Glaive_Combo::Init_State()
{
	__super::Init_State();

	return S_OK;
}

void CPlayer_Glaive_Combo::State_Enter()
{
	m_pActorModelCom->Set_Animation(static_cast<_uint>(PLAYER_STATE::GLAIVE_COMBO), false);
}

void CPlayer_Glaive_Combo::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
	
}

void CPlayer_Glaive_Combo::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	// 22
	// 40

	if (m_bAnimationFinished)
	{
		m_pPlayer->Change_State(PLAYER_STATE::IDLE);
		return;
	}
}

void CPlayer_Glaive_Combo::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CPlayer_Glaive_Combo::State_Exit()
{
}

void CPlayer_Glaive_Combo::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);
}

void CPlayer_Glaive_Combo::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CPlayer_Glaive_Combo::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Player* CPlayer_Glaive_Combo::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEPLAYER_DESC* pDesc)
{
	CPlayer_Glaive_Combo* pGameInstance = new CPlayer_Glaive_Combo(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CPlayer_Glaive_Combo");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CPlayer_Glaive_Combo::Free()
{
	__super::Free();
}
