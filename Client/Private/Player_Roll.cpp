#include "Player_Roll.h"
#include "Body_Player.h"

CPlayer_Roll::CPlayer_Roll(CGameObject* pActor, CModel* pPlayerModelCom, CCollider* pColliderCom,
							CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
							CTransform* pTransformCom, CNavigation* pNavigationCom)
	: CState_Player(pActor, pPlayerModelCom, pColliderCom, pGameObjectDesc, pTransformCom, pNavigationCom)
{
}

HRESULT CPlayer_Roll::Init_State()
{
	__super::Init_State();

	return S_OK;
}

void CPlayer_Roll::State_Enter()
{
	m_pColliderCom->Set_Collider_Off(true);

	m_fRollingTime = 0.f;

	m_pActorModelCom->Set_Animation(static_cast<_uint>(PLAYER_STATE::ROLL), false);
}

void CPlayer_Roll::State_Priority_Update(_float fTimeDelta)
{
	//__super::State_Priority_Update(fTimeDelta);
}

void CPlayer_Roll::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (m_bAnimationFinished)
	{
		m_pPlayer->Change_State(PLAYER_STATE::IDLE);
		return;
	}

	m_fRollingTime += fTimeDelta;

	// 0.7초 동안만 이동
	if (m_fRollingTime <= 0.7f)
	{
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
	}
}


void CPlayer_Roll::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CPlayer_Roll::State_Exit()
{
	m_pColliderCom->Set_Collider_Off(false);
}

void CPlayer_Roll::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);
}

void CPlayer_Roll::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CPlayer_Roll::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Player* CPlayer_Roll::Create(CGameObject* pActor, CModel* pPlayerModelCom, CCollider* pColliderCom,
									CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
									CTransform* pTransformCom, CNavigation* pNavigationCom)
{
	CState_Player* pGameInstance = new CPlayer_Roll(pActor, pPlayerModelCom, pColliderCom, pGameObjectDesc, pTransformCom, pNavigationCom);

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
