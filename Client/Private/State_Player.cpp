#include "State_Player.h"

#include <iostream>

#include "Player.h"
#include "Body_Player.h"

#include "Item.h"
#include "Monster.h"
#include "Monster_Arrow.h"

_float	CState_Player::m_fCombo_ElapsedTime = 0.f;
_bool   CState_Player::m_bCombo1_ColliderOn = { false };
_bool   CState_Player::m_bCombo2_ColliderOn = { false };
_bool   CState_Player::m_bCombo3_ColliderOn = { false };
_bool   CState_Player::m_bCombo1_ColliderOff = { false };
_bool   CState_Player::m_bCombo2_ColliderOff = { false };
_bool   CState_Player::m_bCombo3_ColliderOff = { false };
_bool   CState_Player::m_bCombo1_Finished = { false };
_bool   CState_Player::m_bCombo2_Finished = { false };
_bool   CState_Player::m_bCombo3_Finished = { false };
_bool   CState_Player::m_bComboInitiating = { false };

CState_Player::CState_Player(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEPLAYER_DESC* pDesc)
	: CState(pActor, pGameObjectDesc), m_pStatePlayerDesc(pDesc)
{
}

HRESULT CState_Player::Init_State()
{
	m_pPlayer = dynamic_cast<CPlayer*>(m_pActor);

	m_pPlayerInfo = dynamic_cast<CPlayer::PLAYER_DESC*>(m_pGameObjectInfo);

	m_pActorModelCom	= m_pStatePlayerDesc->pActorModelCom;
	m_pColliderOBBCom	= m_pStatePlayerDesc->pColliderCom;
	m_pTransformCom		= m_pStatePlayerDesc->pTransformCom;
	m_pNavigationCom	= m_pStatePlayerDesc->pNavigationCom;

	// 얘 나중에 Player_BowAction으로 빼기
	m_pArrowPool_Player = m_pStatePlayerDesc->pArrowPool_Player;

	if (nullptr == m_pPlayer || nullptr == m_pPlayerInfo || nullptr == m_pActorModelCom ||
		nullptr == m_pTransformCom || nullptr == m_pNavigationCom || nullptr == m_pColliderOBBCom)
		return E_FAIL;

	return S_OK;
}

void CState_Player::State_Enter()
{
}

void CState_Player::State_Priority_Update(_float fTimeDelta)
{
//#ifdef _DEBUG
//	if (m_pGameInstance->Key_Down(VK_F1))
//		m_bClickLock = !m_bClickLock;
//#endif
}

void CState_Player::State_Update(_float fTimeDelta)
{
	//if (m_pPlayerInfo->Get_CurrentHP() <= 0)
	//	//die

	// std::wcerr << "[플레이어 공격 상태 : " << m_pPlayer->Get_Attacking() << std::endl;

	m_bAnimationFinished = m_pActorModelCom->Play_Animation(fTimeDelta);

	Check_Combo_Timeout(fTimeDelta);
}

void CState_Player::State_Late_Update(_float fTimeDelta)
{
}

void CState_Player::State_Exit()
{
}

void CState_Player::Collision_Enter(CCollider* pOther)
{
	Modify_HP(pOther);

	/*_wstring other = pOther->Get_CollidergGroupTag();

	std::wcerr << "[플레이어와 " << other << " 충돌 Enter]" << std::endl;*/
}

void CState_Player::Collision_Stay(CCollider* pOther)
{
	/*_wstring other = pOther->Get_CollidergGroupTag();

	std::wcerr << "[플레이어와 " << other << " 충돌 Stay]" << std::endl;*/
}

void CState_Player::Collision_Exit(CCollider* pOther)
{
	/*_wstring other = pOther->Get_CollidergGroupTag();

	std::wcerr << "[플레이어와 " << other << " 충돌 Exit]" << std::endl;*/
}

_bool CState_Player::Change_State_To_Idle()
{
	m_pPlayer->Change_State(PLAYER_STATE::IDLE);
	return true;
}

_bool CState_Player::Change_State_To_Walk()
{
	if (m_pGameInstance->Get_Key(VK_LBUTTON) && !bMouseClickLock)
	{
		_float4 fWorldPickedPos = { 0.f, 0.f, 0.f, 1.f };

		// 2. LoungeMap에 피킹 요청 (BoundingBox 충돌 체크)
		if (m_pGameInstance->Picked_Model(fWorldPickedPos, TEXT("GameObject_LoungeMap"),
			LEVEL_GAMEPLAY, TEXT("Layer_BackGround")))
		{
			m_pPlayer->Set_NextPosition(fWorldPickedPos);
			m_pPlayer->Set_Chasing(false, nullptr);
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

_bool CState_Player::Change_State_To_GlaiveCombo()
{
	CTransform* pMonsterTransformCom = m_pPlayer->Get_MonsterTransformCom();

	if (nullptr == pMonsterTransformCom)
		return false;

	_float4 pMonsterPos = {};
	XMStoreFloat4(&pMonsterPos, pMonsterTransformCom->Get_State(CTransform::STATE_POSITION));

	_vector  vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector  vMonsterPos = pMonsterTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector  vVecToMonster = vMonsterPos - vPlayerPos;

	_float   fDistanceSq = XMVectorGetX(XMVector3LengthSq(vVecToMonster));
	const _float& fAttackRangeSq = m_pPlayerInfo->Get_AttackableRange() * m_pPlayerInfo->Get_AttackableRange();
	if (fDistanceSq < fAttackRangeSq)
	{
		m_pPlayer->Change_State(PLAYER_STATE::GLAIVE_COMBO);
		m_pPlayer->Set_Chasing(false, nullptr);

		return true;
	}

	return false;
}

_bool CState_Player::Change_State_To_BowAction()
{
	if (m_pGameInstance->Get_Key(VK_RBUTTON) && !bMouseClickLock)
	{
		_float4 fWorldPickedPos = { 0.f, 0.f, 0.f, 1.f };

		// 2. LoungeMap에 피킹 요청 (BoundingBox 충돌 체크)
		if (m_pGameInstance->Picked_Model(fWorldPickedPos, TEXT("GameObject_LoungeMap"),
			LEVEL_GAMEPLAY, TEXT("Layer_BackGround")))
		{
			m_pPlayer->Set_Shoot_Arrow(true, fWorldPickedPos);
			m_pTransformCom->LookAt(XMLoadFloat4(&fWorldPickedPos));
			m_pPlayer->Change_State(PLAYER_STATE::BOW_ACTION);

			return true;
		}
	}

	return false;
}

_bool CState_Player::Change_State_To_GetHitFront()
{
	m_pPlayer->Change_State(PLAYER_STATE::GET_HIT_FRONT);

	return true;
}

void CState_Player::Modify_HP(CCollider* pOther)
{
	if (TEXT("Monster_Arrow") == pOther->Get_ColliderTag()
		&& pOther->Get_OtherAttacking())
	{
		CMonster_Arrow* pMonsterArrow = dynamic_cast<CMonster_Arrow*>(pOther->Get_OwnerObject());
		m_pPlayerInfo->Modify_CurrentHp(-pMonsterArrow->Get_DealPoint());
	}

	if (TEXT("Monster_Weapon") == pOther->Get_ColliderTag()
		&& pOther->Get_OtherAttacking())
	{
		CItem* pItem = dynamic_cast<CItem*>(pOther->Get_OwnerObject());
		m_pPlayerInfo->Modify_CurrentHp(-pItem->Get_DealPoint());
	}

	if (TEXT("Monster_Body_Hit") == pOther->Get_ColliderTag()
		&& pOther->Get_OtherAttacking())
	{
		CPartObject* pMonsterBody = dynamic_cast<CPartObject*>(pOther->Get_OwnerObject());
		CMonster* pMonster = dynamic_cast<CMonster*>(pMonsterBody->Get_ContainerObject());

		m_pPlayerInfo->Modify_CurrentHp(-pMonster->Get_MonsterInfo()->Get_DealPoint());
	}
}

void CState_Player::Check_Combo_Timeout(_float fTimeDelta)
{
	if (m_bComboInitiating && !m_bCombo3_Finished)
	{
		m_fCombo_ElapsedTime += fTimeDelta;

		if (m_fCombo_ElapsedTime > 3.f)
		{
			Reset_Combo();
		}
	}
}

void CState_Player::Reset_Combo()
{
	m_bComboInitiating	= false;
	m_fCombo_ElapsedTime = 0.f;

	m_bCombo1_ColliderOn = { false };
	m_bCombo2_ColliderOn = { false };
	m_bCombo3_ColliderOn = { false };
	m_bCombo1_ColliderOff = { false };
	m_bCombo2_ColliderOff = { false };
	m_bCombo3_ColliderOff = { false };
	m_bCombo1_Finished = false;
	m_bCombo2_Finished = false;
	m_bCombo3_Finished = false;

	m_fPrevAnimTrackPosition = 0.f;

	//m_pPlayer->Set_Attacking(false);

	std::wcerr << "[콤보 초기화 딩딩딩딩딩~]" << std::endl;
}


void CState_Player::Free()
{
	__super::Free();
}
