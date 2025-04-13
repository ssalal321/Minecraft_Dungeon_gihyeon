#include "Player_Idle.h"
#include "Body_Player.h"

CPlayer_Idle::CPlayer_Idle(CGameObject* pActor, CGameObject* pPartObject, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
						   CTransform* pTransformCom, CNavigation* pNavigationCom)
	: CState_Player(pActor, pPartObject, pGameObjectDesc, pTransformCom, pNavigationCom)
{
}

HRESULT CPlayer_Idle::Init_State()
{
	__super::Init_State();

	return S_OK;
}

void CPlayer_Idle::State_Enter()
{
}

void CPlayer_Idle::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CPlayer_Idle::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	m_pBodyPlayerModelCom->Set_Animation(PLAYER_STATE::IDLE_GLAIVE, true);
}

void CPlayer_Idle::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CPlayer_Idle::State_Exit()
{
}

CState_Player* CPlayer_Idle::Create(CGameObject* pActor, CGameObject* pPartObject, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
							 CTransform* pTransformCom, CNavigation* pNavigationCom)
{
	CState_Player* pGameInstance = new CPlayer_Idle(pActor, pPartObject, pGameObjectDesc, pTransformCom, pNavigationCom);

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
