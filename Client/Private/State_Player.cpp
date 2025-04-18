#include "State_Player.h"

#include <iostream>

#include "Player.h"
#include "Body_Player.h"

_bool CState_Player::m_bAnimationFinished = false;

CState_Player::CState_Player(CGameObject* pActor, CModel* pPlayerModelCom, CCollider* pColliderCom,
							CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
							CTransform* pTransformCom, CNavigation* pNavigationCom)
	: CState(pActor, pPlayerModelCom, pColliderCom, pGameObjectDesc, pTransformCom, pNavigationCom)
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
	if (m_pGameInstance->Key_Down(VK_F1))
		m_bClickLock = !m_bClickLock;

	if (m_pGameInstance->Get_Key(VK_LBUTTON) && !m_bClickLock)
	{
		_float4 fWorldPickedPos = { 0.f, 0.f, 0.f, 1.f };

		// 2. LoungeMap에 피킹 요청 (BoundingBox 충돌 체크)
		if (m_pGameInstance->Picked_Model(fWorldPickedPos, TEXT("Prototype_GameObject_LoungeMap"),
											LEVEL_GAMEPLAY, TEXT("Layer_BackGround")))
		{
			m_pPlayer->Set_NextPosition(fWorldPickedPos);
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

void CState_Player::Collision_Enter(CCollider* pOther)
{
	_wstring other = pOther->Get_OwnerTag();

	std::wcerr << "[플레이어와 " << other << " 충돌 Enter]" << std::endl;
}

void CState_Player::Collision_Stay(CCollider* pOther)
{
	_wstring other = pOther->Get_OwnerTag();

	std::wcerr << "[플레이어와 " << other << " 충돌 Stay]" << std::endl;
}

void CState_Player::Collision_Exit(CCollider* pOther)
{
	_wstring other = pOther->Get_OwnerTag();

	std::wcerr << "[플레이어와 " << other << " 충돌 Exit]" << std::endl;
}


void CState_Player::Free()
{
	__super::Free();
}
