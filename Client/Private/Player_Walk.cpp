#include "Player_Walk.h"

#include <iostream>
#include <ostream>

#include "Player.h"
#include "Body_Player.h"

CPlayer_Walk::CPlayer_Walk(CGameObject* pActor, CGameObject* pPartObject, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, CNavigation* pNavigationCom)
	: CState(pActor, pPartObject, pGameObjectDesc), m_pNavigationCom(pNavigationCom)
{
}

HRESULT CPlayer_Walk::Init_State()
{
	m_pPlayer = dynamic_cast<CPlayer*>(m_pActor);
	m_pTransformCom = dynamic_cast<CTransform*>(m_pPlayer->Find_Component(TEXT("Com_Transform")));

	m_pBodyPlayer = dynamic_cast<CBody_Player*>(m_pPartObject);
	m_pBodyPlayerModelCom = dynamic_cast<CModel*>(m_pBodyPlayer->Find_Component(TEXT("Com_Model")));

	m_pPlayerDesc = dynamic_cast<CPlayer::PLAYER_DESC*>(m_pGameObjectDesc);

	if (nullptr == m_pPlayer || nullptr == m_pTransformCom || nullptr == m_pPlayerDesc)
		return E_FAIL;

	return S_OK;
}

void CPlayer_Walk::State_Enter()
{
	m_pBodyPlayerModelCom->Set_Animation(PLAYER_STATE::WALK_GLAIVE, true, 1.6f);
}

void CPlayer_Walk::State_Priority_Update(_float fTimeDelta)
{
}

// Player_Walk
void CPlayer_Walk::State_Update(_float fTimeDelta)
{
	_vector  vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector  vNextPos = XMLoadFloat4(&m_pPlayer->Get_NextPosition());
	//XMVectorSetY(vNextPos, 0.f);

	_vector  vToTarget = vNextPos - vCurPos;
	XMVectorSetY(vToTarget, 0.f);

	_float fDist = XMVectorGetX(XMVector3Length(vToTarget));

	if (fDist < 0.1f)
	{
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNextPos);

		m_pPlayer->Change_State(PLAYER_STATE::IDLE);

		return;
	}

	m_pTransformCom->LookAt(vNextPos);

	m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);

	std::cerr << "플레이어 위치 Y: " << m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] << std::endl;
}

void CPlayer_Walk::State_Late_Update(_float fTimeDelta)
{
}

void CPlayer_Walk::State_Exit()
{
}

CState* CPlayer_Walk::Create(CGameObject* pActor, CGameObject* pPartObject, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, CNavigation* pNavigationCom)
{
	CState* pGameInstance = new CPlayer_Walk(pActor, pPartObject, pGameObjectDesc, pNavigationCom);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CPlayer_Walk");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CPlayer_Walk::Free()
{
	__super::Free();
}
