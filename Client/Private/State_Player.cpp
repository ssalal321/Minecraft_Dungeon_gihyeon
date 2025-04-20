#include "State_Player.h"

#include <iostream>

#include "Player.h"
#include "Body_Player.h"


CState_Player::CState_Player(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEPLAYER_DESC* pDesc)
	: CState(pActor, pGameObjectDesc), m_pStatePlayerDesc(pDesc)
{
}

HRESULT CState_Player::Init_State()
{
	m_pPlayer = dynamic_cast<CPlayer*>(m_pActor);

	m_pPlayerDesc = dynamic_cast<CPlayer::PLAYER_DESC*>(m_pGameObjectDesc);

	m_pActorModelCom	= m_pStatePlayerDesc->pActorModelCom;
	m_pColliderOBBCom	= m_pStatePlayerDesc->pColliderOBBCom;
	m_pTransformCom		= m_pStatePlayerDesc->pTransformCom;
	m_pNavigationCom	= m_pStatePlayerDesc->pNavigationCom;

	if (nullptr == m_pPlayer || nullptr == m_pPlayerDesc || nullptr == m_pActorModelCom ||
		nullptr == m_pTransformCom || nullptr == m_pNavigationCom || nullptr == m_pColliderOBBCom)
		return E_FAIL;

	return S_OK;
}

void CState_Player::State_Enter()
{
}

void CState_Player::State_Priority_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	if (m_pGameInstance->Key_Down(VK_F1))
		m_bClickLock = !m_bClickLock;
#endif
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
	/*_wstring other = pOther->Get_OwnerTag();

	std::wcerr << "[플레이어와 " << other << " 충돌 Enter]" << std::endl;*/
}

void CState_Player::Collision_Stay(CCollider* pOther)
{
	/*_wstring other = pOther->Get_OwnerTag();

	std::wcerr << "[플레이어와 " << other << " 충돌 Stay]" << std::endl;*/
}

void CState_Player::Collision_Exit(CCollider* pOther)
{
	/*_wstring other = pOther->Get_OwnerTag();

	std::wcerr << "[플레이어와 " << other << " 충돌 Exit]" << std::endl;*/
}

_bool CState_Player::Change_State_To_Idle()
{
	return true;
}

_bool CState_Player::Change_State_To_Walk()
{
	if (m_pGameInstance->Get_Key(VK_LBUTTON) && !m_bClickLock)
	{
		_float4 fWorldPickedPos = { 0.f, 0.f, 0.f, 1.f };

		// 2. LoungeMap에 피킹 요청 (BoundingBox 충돌 체크)
		if (m_pGameInstance->Picked_Model(fWorldPickedPos, TEXT("Prototype_GameObject_LoungeMap"),
			LEVEL_GAMEPLAY, TEXT("Layer_BackGround")))
		{
			m_pPlayer->Set_NextPosition(fWorldPickedPos);
			m_pPlayer->Change_State(PLAYER_STATE::WALK);

			return true;
		}
	}

	return false;
}

_bool CState_Player::Change_State_To_Roll()
{
	if (m_pGameInstance->Key_Down(VK_SPACE))
	{
		m_pPlayer->Change_State(PLAYER_STATE::ROLL);

		return true;
	}

	return false;
}

_bool CState_Player::Change_State_To_Attack()
{
	if (m_pPlayer->Get_Chasing())
	{
		CTransform* pMonsterTransformCom = m_pPlayer->Get_MonsterTransformCom();
		_float4 pMonsterPos = {};
		XMStoreFloat4(&pMonsterPos, pMonsterTransformCom->Get_State(CTransform::STATE_POSITION));

		_vector  vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector  vMonsterPos = pMonsterTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector  vVecToMonster = vMonsterPos - vPlayerPos;

		_float fDistanceSq = XMVectorGetX(XMVector3LengthSq(vVecToMonster));
		if (fDistanceSq < 16.f) // 4.f * 4.f
		{
			m_pPlayer->Set_Chasing(false, nullptr);
			m_pPlayer->Change_State(PLAYER_STATE::GLAIVE_COMBO);

			return true;
		}
	}

	return false;

}

void CState_Player::Free()
{
	__super::Free();
}
