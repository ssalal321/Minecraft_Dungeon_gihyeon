#include "Player_Idle.h"
#include "Body_Player.h"

CPlayer_Idle::CPlayer_Idle(CGameObject* pActor, CGameObject* pPartObject, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc)
	: CState(pActor, pPartObject, pGameObjectDesc)
{
}

HRESULT CPlayer_Idle::Init_State()
{
	m_pPlayer = dynamic_cast<CPlayer*>(m_pActor);
	m_pTransformCom = dynamic_cast<CTransform*>(m_pPlayer->Find_Component(TEXT("Com_Transform")));

	m_pBodyPlayer = dynamic_cast<CBody_Player*>(m_pPartObject);
	m_pBodyPlayerModelCom = dynamic_cast<CModel*>(m_pBodyPlayer->Find_Component(TEXT("Com_Model")));

	m_pPlayerDesc = static_cast<CPlayer::PLAYER_DESC*>(m_pGameObjectDesc);

	if (nullptr == m_pPlayer || nullptr == m_pTransformCom || nullptr == m_pPlayerDesc)
		return E_FAIL;

	return S_OK;
}

void CPlayer_Idle::State_Enter()
{
}

void CPlayer_Idle::State_Priority_Update(_float fTimeDelta)
{
}

void CPlayer_Idle::State_Update(_float fTimeDelta)
{
	m_pBodyPlayerModelCom->Set_Animation(PLAYER_STATE::IDLE_GLAIVE, true);
}

void CPlayer_Idle::State_Late_Update(_float fTimeDelta)
{
}

void CPlayer_Idle::State_Exit()
{
}

CState* CPlayer_Idle::Create(CGameObject* pActor, CGameObject* pPartObject, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc)
{
	CState* pGameInstance = new CPlayer_Idle(pActor, pPartObject, pGameObjectDesc);

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
