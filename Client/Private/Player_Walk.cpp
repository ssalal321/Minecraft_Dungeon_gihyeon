#include "Player_Walk.h"
#include "Body_Player.h"

CPlayer_Walk::CPlayer_Walk(CGameObject* pActor, CGameObject* pPartObject, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
							CTransform* pTransformCom, CNavigation* pNavigationCom)
	: CState_Player(pActor, pPartObject, pGameObjectDesc, pTransformCom, pNavigationCom)
{
}

HRESULT CPlayer_Walk::Init_State()
{
	__super::Init_State();

	return S_OK;
}

void CPlayer_Walk::State_Enter()
{
	m_pBodyPlayerModelCom->Set_Animation(PLAYER_STATE::WALK_GLAIVE, true, 1.6f);
}

void CPlayer_Walk::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

// Player_Walk
void CPlayer_Walk::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

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
}

void CPlayer_Walk::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CPlayer_Walk::State_Exit()
{
}

CState_Player* CPlayer_Walk::Create(CGameObject* pActor, CGameObject* pPartObject, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
									CTransform* pTransformCom, CNavigation* pNavigationCom)
{
	CState_Player* pGameInstance = new CPlayer_Walk(pActor, pPartObject, pGameObjectDesc, pTransformCom, pNavigationCom);

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
