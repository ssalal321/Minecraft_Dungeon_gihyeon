#include "State_Player.h"
#include "Player.h"
#include "Body_Player.h"

_bool CState_Player::m_bAnimationFinished = false;

CState_Player::CState_Player(CGameObject* pActor, CModel* pPlayerModelCom, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
							 CTransform* pTransformCom, CNavigation* pNavigationCom)
	: CState(pActor, pPlayerModelCom, pGameObjectDesc, pTransformCom, pNavigationCom)
{
}

HRESULT CState_Player::Init_State()
{
	m_pPlayer = dynamic_cast<CPlayer*>(m_pActor);

	m_pPlayerDesc = dynamic_cast<CPlayer::PLAYER_DESC*>(m_pGameObjectDesc);

	if (nullptr == m_pPlayer || nullptr == m_pPlayerDesc ||
		nullptr == m_pTransformCom || nullptr == m_pNavigationCom)
		return E_FAIL;	

	return S_OK;
}

void CState_Player::State_Enter()
{
}

void CState_Player::State_Priority_Update(_float fTimeDelta)
{
	if (m_pGameInstance->Get_Key(VK_LBUTTON))
	{
		_float3 fWorldPickedPos = {};

		// 2. LoungeMap에 피킹 요청 (BoundingBox 충돌 체크)
		if (m_pGameInstance->Picked_Model(fWorldPickedPos, TEXT("Prototype_GameObject_LoungeMap"),
											LEVEL_GAMEPLAY, TEXT("Layer_BackGround")))
		{
			m_pPlayer->Set_NextPosition({ fWorldPickedPos.x, fWorldPickedPos.y, fWorldPickedPos.z, 1.f });
			m_pPlayer->Change_State(PLAYER_STATE::WALK);
		}
	}

	if (m_pGameInstance->Key_Down(VK_SPACE))
	{
		m_pPlayer->Change_State(PLAYER_STATE::ROLL);
	}	
}

void CState_Player::State_Update(_float fTimeDelta)
{
	m_bAnimationFinished = m_pActorModelCom->Play_Animation(fTimeDelta);
}

void CState_Player::State_Late_Update(_float fTimeDelta)
{
	
}

void CState_Player::State_Exit()
{
}

//CState* CState_Player::Create(CGameObject* pActor, CGameObject* pPartObject, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
//							  CTransform* pTransformCom, CNavigation* pNavigationCom)
//{
//	CState* pGameInstance = new CState_Player(pActor, pPartObject, pGameObjectDesc, pTransformCom, pNavigationCom);
//
//	if (FAILED(pGameInstance->Init_State()))
//	{
//		MSG_BOX("Failed to Create : CState_Player");
//		Safe_Release(pGameInstance);
//	}
//
//	return pGameInstance;
//}

void CState_Player::Free()
{
	__super::Free();
}
