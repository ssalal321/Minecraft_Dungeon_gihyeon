#include "State_Player.h"

#include <iostream>

#include "Player.h"
#include "Body_Player.h"
#include "InventoryData.h"

#include "Item.h"
#include "Level_Loading.h"
#include "Level_SoggySwamp.h"
#include "Monster.h"
#include "Monster_Arrow.h"
#include "Slime_Cauldron_Bullet.h"

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
	m_pColliderBigCom	= m_pStatePlayerDesc->pColliderBigCom;
	m_pColliderSmallCom = m_pStatePlayerDesc->pColliderSmallCom;
	m_pTransformCom		= m_pStatePlayerDesc->pTransformCom;
	//m_pNavigationCom	= m_pStatePlayerDesc->pNavigationCom;

	// 얘 나중에 Player_BowAction으로 빼기
	m_pArrowPool_Player = m_pStatePlayerDesc->pArrowPool_Player;

	if (nullptr == m_pPlayer || nullptr == m_pPlayerInfo || nullptr == m_pActorModelCom ||
		nullptr == m_pTransformCom /*|| nullptr == m_pNavigationCom*/ || nullptr == m_pColliderBigCom)
		return E_FAIL;

	return S_OK;
}

void CState_Player::State_Enter()
{
}

void CState_Player::State_Priority_Update(_float fTimeDelta)
{

}

void CState_Player::State_Update(_float fTimeDelta)
{
	m_bAnimationFinished = m_pActorModelCom->Play_Animation(fTimeDelta);

	if (m_pGameInstance->Key_Down('E') && false == m_pPlayerInfo->Get_HPCoolDowning())
	{
		m_pPlayerInfo->Modify_CurrentHp(m_pPlayerInfo->Get_MaxHP());
		m_pPlayerInfo->Start_HPCoolDown(true);
	}

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
}

void CState_Player::Collision_Stay(CCollider* pOther)
{
}

void CState_Player::Collision_Exit(CCollider* pOther)
{

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

		_uint	currentLevel = m_pGameInstance->Get_CurrentLevelIndex();
		_wstring	mapTag = {};
		switch (currentLevel)
		{
		case LEVEL_LOUNGE:
			mapTag = TEXT("GameObject_LoungeMap");
			break;

		case LEVEL_SOGGYSWAMP:
			mapTag = TEXT("GameObject_SoggySwampMap");
			break;
		}

		// Map에 피킹(BoundingBox 충돌 체크)
		if (m_pGameInstance->Picked_Model(fWorldPickedPos, mapTag,
			currentLevel, TEXT("Layer_BackGround")))
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
	if (m_pGameInstance->Key_Down(VK_SPACE) && false == m_pPlayerInfo->Get_RollCoolDowning())
	{
		m_pPlayer->Change_State(PLAYER_STATE::ROLL);

		return true;
	}

	return false;
}

_bool CState_Player::Change_State_To_GlaiveCombo()
{
	if (nullptr == m_pPlayer->Find_PartObject(TEXT("Part_Weapon_Melee")))
	{
		CInventoryData* pInventory = m_pPlayer->Get_InventoryData();
		if (pInventory)
		{
			CItem* pGearItem = pInventory->Get_GearItem(0);
			if (!pGearItem || pGearItem->Get_GameObjectTag() != TEXT("GameObject_GlaiveSteel"))
				return false;
		}
	}


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
	if (nullptr == m_pPlayer->Find_PartObject(TEXT("Part_Weapon_Ranged")))
		return false;

	if (m_pGameInstance->Get_Key(VK_RBUTTON) && !bMouseClickLock)
	{
		_float4 fWorldPickedPos = { 0.f, 0.f, 0.f, 1.f };

		_uint	currentLevel = m_pGameInstance->Get_CurrentLevelIndex();
		_wstring	mapTag = {};
		switch (currentLevel)
		{
		case LEVEL_LOUNGE:
			mapTag = TEXT("GameObject_LoungeMap");
			break;

		case LEVEL_SOGGYSWAMP:
			mapTag = TEXT("GameObject_SoggySwampMap");
			break;
		}

		// 2. LoungeMap에 피킹 요청 (BoundingBox 충돌 체크)
		if (m_pGameInstance->Picked_Model(fWorldPickedPos, mapTag,
			currentLevel, TEXT("Layer_BackGround")))
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
		&& pOther->Get_Other_Collision_Activated())
	{
		CMonster_Arrow* pMonsterArrow = dynamic_cast<CMonster_Arrow*>(pOther->Get_OwnerObject());
		m_pPlayerInfo->Modify_CurrentHp(-pMonsterArrow->Get_DealPoint());

		m_pPlayer->Render_DamageFont(pMonsterArrow->Get_DealPoint(), 4.f);
	}

	if (TEXT("Slime_Cauldron_Bullet") == pOther->Get_ColliderTag()
		&& pOther->Get_Other_Collision_Activated())
	{
		CSlime_Cauldron_Bullet* pMonsterBullet = dynamic_cast<CSlime_Cauldron_Bullet*>(pOther->Get_OwnerObject());
		m_pPlayerInfo->Modify_CurrentHp(-pMonsterBullet->Get_DealPoint());

		m_pPlayer->Render_DamageFont(pMonsterBullet->Get_DealPoint(), 4.f);
	}

	if (TEXT("Monster_Weapon") == pOther->Get_ColliderTag()
		&& pOther->Get_Other_Collision_Activated())
	{
		CItem* pItem = dynamic_cast<CItem*>(pOther->Get_OwnerObject());
		m_pPlayerInfo->Modify_CurrentHp(-pItem->Get_DealPoint());

		m_pPlayer->Render_DamageFont(pItem->Get_DealPoint(), 4.f);
	}

	if (TEXT("Monster_Body_Hit") == pOther->Get_ColliderTag()
		&& pOther->Get_Other_Collision_Activated())
	{
		CPartObject* pMonsterBody = dynamic_cast<CPartObject*>(pOther->Get_OwnerObject());
		CMonster* pMonster = dynamic_cast<CMonster*>(pMonsterBody->Get_ContainerObject());

		m_pPlayerInfo->Modify_CurrentHp(-pMonster->Get_MonsterInfo()->Get_DealPoint());

		m_pPlayer->Render_DamageFont(pMonster->Get_MonsterInfo()->Get_DealPoint(), 4.f);
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

	CItem* pMeleeItem = dynamic_cast<CItem*>(m_pPlayer->Find_PartObject(TEXT("Part_Weapon_Melee")));
	if (nullptr != pMeleeItem)
	{
		pMeleeItem->Reset_DealPoint();
	}

	//m_pPlayer->Set_Attacking(false);

	//std::wcerr << "[콤보 초기화 딩딩딩딩딩~]" << std::endl;
}


void CState_Player::Free()
{
	__super::Free();
}
