#include "Player_Roll.h"
#include "Body_Player.h"

CPlayer_Roll::CPlayer_Roll(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEPLAYER_DESC* pDesc)
	: CState_Player(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CPlayer_Roll::Init_State()
{
	__super::Init_State();

	return S_OK;
}

void CPlayer_Roll::State_Enter()
{
	m_bRollStarted = false;
	m_pColliderSmallCom->Set_ColliderActive(false);
	m_pColliderBigCom->Set_ColliderActive(false);

	m_pActorModelCom->Set_Animation(static_cast<_uint>(PLAYER_STATE::ROLL), false, 1.1f);
}

void CPlayer_Roll::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CPlayer_Roll::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (m_bAnimationFinished)
	{
		if (Change_State_To_BowAction())
			return;

		if (Change_State_To_Walk())
			return;

		if (Change_State_To_Idle())
			return;
	}

	_float  fAnimCurTrackPos = m_pActorModelCom->Get_AnimCurrentTrackPosition();

	if (!m_bRollStarted && fAnimCurTrackPos > 2.f)
	{
		m_bRollStarted = true;
	}

	if (m_bRollStarted && fAnimCurTrackPos <= 19.f)
	{
		CNavigation* pNavigationCom = dynamic_cast<CNavigation*>(m_pPlayer->Find_Component(TEXT("Com_Navigation")));
		m_pTransformCom->Go_Straight(fTimeDelta, pNavigationCom, 1.7f);
	}
}


void CPlayer_Roll::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CPlayer_Roll::State_Exit()
{
	m_pColliderSmallCom->Set_ColliderActive(true);
	m_pColliderBigCom->Set_ColliderActive(true);
}

void CPlayer_Roll::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);

	// 그냥 Roll에만 ModifyHP 안넣으면 된다.
	// 하지만 충돌 연산 잠시라도 줄일 겸 그냥 Collider 끄는 방식 채택하겟다.
}

void CPlayer_Roll::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CPlayer_Roll::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Player* CPlayer_Roll::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEPLAYER_DESC* pDesc)
{
	CPlayer_Roll* pGameInstance = new CPlayer_Roll(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CPlayer_Roll");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CPlayer_Roll::Free()
{
	__super::Free();
}
