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
	m_pColliderOBBCom->Set_ColliderActive(true);

	m_fRollingTime = 0.f;

	m_pActorModelCom->Set_Animation(static_cast<_uint>(PLAYER_STATE::ROLL), false, 1.3f);
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

	m_fRollingTime += fTimeDelta;

	m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom, 3.5f);
}


void CPlayer_Roll::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CPlayer_Roll::State_Exit()
{
	m_pColliderOBBCom->Set_ColliderActive(false);
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
