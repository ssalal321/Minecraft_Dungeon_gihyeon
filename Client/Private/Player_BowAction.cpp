#include "Player_BowAction.h"

#include "Player_Arrow.h"
#include "ArrowPool_Player.h"
#include "Body_Player.h"

#define SHOOT_ARROW  25.f

CPlayer_BowAction::CPlayer_BowAction(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEPLAYER_DESC* pDesc)
	: CState_Player(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CPlayer_BowAction::Init_State()
{
	__super::Init_State();

	return S_OK;
}

void CPlayer_BowAction::State_Enter()
{
	m_pActorModelCom->Set_Animation(static_cast<_uint>(PLAYER_STATE::BOW_ACTION), false, 2.f);
}

void CPlayer_BowAction::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CPlayer_BowAction::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (m_bAnimationFinished)
	{
		if (Change_State_To_Idle())
			return;

		m_bShotArrow = false;
	}

	_float fAnimCurTrackPos = m_pActorModelCom->Get_AnimCurrentTrackPosition();

	if (!m_bShotArrow && SHOOT_ARROW <= fAnimCurTrackPos)
		Shoot_Arrow();
}

void CPlayer_BowAction::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CPlayer_BowAction::State_Exit()
{
	m_bShotArrow = false;

	m_pPlayer->Set_Shoot_Arrow(false, { 0.f, 0.f, 0.f, 1.f });
}

void CPlayer_BowAction::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);
}

void CPlayer_BowAction::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CPlayer_BowAction::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

void CPlayer_BowAction::Shoot_Arrow()
{
	CPlayer_Arrow* pPlayerArrow = m_pArrowPool_Player->Get_Arrow(m_pPlayerInfo->Get_Arrow_DealPoint());

	_float4 playerPos = {};
	XMStoreFloat4(&playerPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_bShotArrow = true;
	m_pPlayerInfo->iArrowNum -= 1;
	pPlayerArrow->Shoot(playerPos, m_pPlayer->Get_PickedPosition());
}

CState_Player* CPlayer_BowAction::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEPLAYER_DESC* pDesc)
{
	CPlayer_BowAction* pGameInstance = new CPlayer_BowAction(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CPlayer_BowAction");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CPlayer_BowAction::Free()
{
	__super::Free();
}
