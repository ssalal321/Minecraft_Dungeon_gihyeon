#include "Player_Glaive_Combo.h"
#include "Body_Player.h"

#define GLAIVE_COMBO1  22.f
#define GLAIVE_COMBO2  40.f

CPlayer_Glaive_Combo::CPlayer_Glaive_Combo(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEPLAYER_DESC* pDesc)
	: CState_Player(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CPlayer_Glaive_Combo::Init_State()
{
	__super::Init_State();

	return S_OK;
}

void CPlayer_Glaive_Combo::State_Enter()
{
	m_pPlayer->Set_Attacking(true);

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

	_float fAnimCurTrackPos = m_pActorModelCom->Get_AnimCurrentTrackPosition();

	if (!m_bCombo1_Finished && GLAIVE_COMBO1 <= fAnimCurTrackPos)  // GLAIVE_COMBO1까지만
	{
		m_fPrevAnimTrackPosition = GLAIVE_COMBO1;
		m_bCombo1_Finished = true;

		m_bComboInitiating = true;
		m_fCombo_ElapsedTime = 0.f;

		m_pPlayer->Change_State(PLAYER_STATE::IDLE);
		return;
	}

	if (!m_bCombo2_Finished && GLAIVE_COMBO2 <= fAnimCurTrackPos)  // GLAIVE_COMBO2까지만
	{
		m_fPrevAnimTrackPosition = GLAIVE_COMBO2;
		m_bCombo2_Finished = true;

		m_bComboInitiating = true;
		m_fCombo_ElapsedTime = 0.f;

		m_pPlayer->Change_State(PLAYER_STATE::IDLE);
		return;
	}

	if (!m_bCombo3_Finished && m_bAnimationFinished)  // GLAIVE_COMBO3 끝나면
	{
		m_bCombo3_Finished = true;

		m_pPlayer->Change_State(PLAYER_STATE::IDLE);
		return;
	}
}

void CPlayer_Glaive_Combo::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CPlayer_Glaive_Combo::State_Exit()
{
	m_pPlayer->Set_Attacking(false);
	m_pPlayer->Set_Chasing(false, nullptr);

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
