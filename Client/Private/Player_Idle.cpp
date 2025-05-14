#include "Player_Idle.h"

#include <iostream>

#include "Body_Player.h"

CPlayer_Idle::CPlayer_Idle(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEPLAYER_DESC* pDesc)
	: CState_Player(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CPlayer_Idle::Init_State()
{
	__super::Init_State();

	return S_OK;
}

void CPlayer_Idle::State_Enter()
{
	std::cerr << "Player_Idle" << "\n";
	/*CPartObject* pItem = m_pPlayer->Find_PartObject(TEXT("Part_Weapon_Melee"));

	if (pItem && pItem->Get_GameObjectTag() == TEXT("GameObject_GlaiveSteel"))
		m_pActorModelCom->Set_Animation(static_cast<_uint>(PLAYER_STATE::IDLE_GLAIVE), true);

	else
		m_pActorModelCom->Set_Animation(static_cast<_uint>(PLAYER_STATE::IDLE), true);*/
}

void CPlayer_Idle::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);

	CPartObject* pItem = m_pPlayer->Find_PartObject(TEXT("Part_Weapon_Melee"));

	if (pItem && pItem->Get_GameObjectTag() == TEXT("GameObject_GlaiveSteel"))
		m_pActorModelCom->Set_Animation(static_cast<_uint>(PLAYER_STATE::IDLE_GLAIVE), true);

	else
		m_pActorModelCom->Set_Animation(static_cast<_uint>(PLAYER_STATE::IDLE), true);

}

void CPlayer_Idle::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (Change_State_To_Roll())
		return;

	if (Change_State_To_BowAction())
		return;

	/*if (Change_State_To_GlaiveCombo())
		return;*/

	if (Change_State_To_Walk())
		return;
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

	//Change_State_To_GetHitFront();
}

void CPlayer_Idle::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CPlayer_Idle::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Player* CPlayer_Idle::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEPLAYER_DESC* pDesc)
{
	CPlayer_Idle* pGameInstance = new CPlayer_Idle(pActor, pGameObjectDesc, pDesc);

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
