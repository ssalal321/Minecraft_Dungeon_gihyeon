#include "Player_Glaive_Combo.h"

#include <iostream>

#include "Body_Player.h"

#define GLAIVE_COMBO1_AttackOn  10.f
#define GLAIVE_COMBO1_AttackOff  19.f
#define GLAIVE_COMBO2_AttackOn  30.f
#define GLAIVE_COMBO2_AttackOff  37.f
#define GLAIVE_COMBO3_AttackOn  61.f
#define GLAIVE_COMBO3_AttackOff  72.f
#define GLAIVE_COMBO1_Finish  22.f
#define GLAIVE_COMBO2_Finish  40.f
#define GLAIVE_COMBO3_Finish  85.f

CPlayer_Glaive_Combo::CPlayer_Glaive_Combo(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEPLAYER_DESC* pDesc)
	: CState_Player(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CPlayer_Glaive_Combo::Init_State()
{
	__super::Init_State();

	m_pGlaiveCollider = dynamic_cast<CCollider*>(m_pPlayer->Find_Part_Component(TEXT("Part_Weapon_Glaive"),
																	 TEXT("Com_Collider_Sphere")));

	return S_OK;
}

void CPlayer_Glaive_Combo::State_Enter()
{
	//m_fAnimTimer = 0.f;

	m_pTransformCom->LookAt(m_pPlayer->Get_MonsterTransformCom()->Get_State(CTransform::STATE_POSITION));

	if (!m_bCombo1_Finished)  // 처음 들어올 때
	{
		m_pActorModelCom->Set_Animation(static_cast<_uint>(PLAYER_STATE::GLAIVE_COMBO), false, 1.3f);
		return;
	}

	if (m_bCombo1_Finished)  // 두 번째, 세 번째
	{
		m_pActorModelCom->Link_AnimationCombo(static_cast<_uint>(PLAYER_STATE::GLAIVE_COMBO), m_fPrevAnimTrackPosition, false, 1.3f);
	}
}

void CPlayer_Glaive_Combo::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
	
}

void CPlayer_Glaive_Combo::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	m_pTransformCom->LookAt(XMLoadFloat4(&m_pPlayer->Get_MonsterPickedPosition()));

	_float  fAnimCurTrackPos = m_pActorModelCom->Get_AnimCurrentTrackPosition();
	

	if (!m_bCombo1_ColliderOn && GLAIVE_COMBO1_AttackOn <= fAnimCurTrackPos)
	{
		m_pPlayer->Set_Attacking(true);
		m_pGlaiveCollider->Set_ColliderActive(true);
		m_bCombo1_ColliderOn = true;
	}
		
	if (!m_bCombo1_ColliderOff && GLAIVE_COMBO1_AttackOff <= fAnimCurTrackPos)
	{
		m_pPlayer->Set_Attacking(false);
		m_pGlaiveCollider->Set_ColliderActive(false);
		m_bCombo1_ColliderOff = true;
	}

	if (!m_bCombo2_ColliderOn && GLAIVE_COMBO2_AttackOn <= fAnimCurTrackPos)
	{
		m_pPlayer->Set_Attacking(true);
		m_pGlaiveCollider->Set_ColliderActive(true);
		m_bCombo2_ColliderOn = true;
	}

	if (!m_bCombo2_ColliderOff && GLAIVE_COMBO2_AttackOff <= fAnimCurTrackPos)
	{
		m_pPlayer->Set_Attacking(false);
		m_pGlaiveCollider->Set_ColliderActive(false);
		m_bCombo2_ColliderOff = true;
	}

	if (!m_bCombo3_ColliderOn && GLAIVE_COMBO3_AttackOn <= fAnimCurTrackPos)
	{
		m_pPlayer->Set_Attacking(true);
		m_pGlaiveCollider->Set_ColliderActive(true);
		m_bCombo3_ColliderOn = true;
	}

	if (!m_bCombo3_ColliderOff && GLAIVE_COMBO3_AttackOff <= fAnimCurTrackPos)
	{
		m_pPlayer->Set_Attacking(false);
		m_pGlaiveCollider->Set_ColliderActive(false);
		m_bCombo3_ColliderOff = true;
	}

	if (!m_bCombo1_Finished && GLAIVE_COMBO1_Finish <= fAnimCurTrackPos)  // GLAIVE_COMBO1까지만
	{
		m_fPrevAnimTrackPosition = GLAIVE_COMBO1_Finish;
		m_bCombo1_Finished = true;
		std::wcerr << "[콤보 1 끝]" << std::endl;
	}

	if (!m_bCombo2_Finished && GLAIVE_COMBO2_Finish <= fAnimCurTrackPos)  // GLAIVE_COMBO2까지만
	{
		m_fPrevAnimTrackPosition = GLAIVE_COMBO2_Finish;
		m_bCombo2_Finished = true;
		std::wcerr << "[콤보 2 끝]" << std::endl;
	}

	if (!m_bCombo3_Finished && GLAIVE_COMBO3_Finish <= fAnimCurTrackPos)  // GLAIVE_COMBO3 끝나면
	{
		m_bCombo3_Finished = true;
		std::wcerr << "[콤보 3 끝]" << std::endl;

		m_pPlayer->Change_State(PLAYER_STATE::IDLE);
		std::wcerr << "[안녕히 계세요 여러분]" << std::endl;

		return;
	}


	if (false == m_pPlayer->Get_Chasing())
	{
		if (m_bCombo1_Finished && fAnimCurTrackPos < GLAIVE_COMBO1_Finish + 1.5f)
		{
			m_pPlayer->Change_State(PLAYER_STATE::IDLE);
			std::wcerr << "[안녕히 계세요 여러분]" << std::endl;

			m_bComboInitiating = true;
			m_fCombo_ElapsedTime = 0.f;
		}

		if (m_bCombo2_Finished && fAnimCurTrackPos < GLAIVE_COMBO2_Finish + 1.5f)
		{
			m_pPlayer->Change_State(PLAYER_STATE::IDLE);
			std::wcerr << "[안녕히 계세요 여러분]" << std::endl;

			m_bComboInitiating = true;
			m_fCombo_ElapsedTime = 0.f;
		}
	}
}

void CPlayer_Glaive_Combo::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CPlayer_Glaive_Combo::State_Exit()
{
	m_pPlayer->Set_Chasing(false, nullptr);
	m_pPlayer->Set_Attacking(false);
	m_pGlaiveCollider->Set_ColliderActive(false);

	if (m_bCombo3_Finished)
	{
		Reset_Combo();
	}
}

void CPlayer_Glaive_Combo::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);
}

void CPlayer_Glaive_Combo::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CPlayer_Glaive_Combo::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Player* CPlayer_Glaive_Combo::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEPLAYER_DESC* pDesc)
{
	CPlayer_Glaive_Combo* pGameInstance = new CPlayer_Glaive_Combo(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CPlayer_Glaive_Combo");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CPlayer_Glaive_Combo::Free()
{
	__super::Free();
}
