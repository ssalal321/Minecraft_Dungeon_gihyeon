#include "Player_BowAction.h"
#include "Body_Player.h"


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
	m_fAnimTimer = 0.f;

	m_pTransformCom->LookAt(m_pPlayer->Get_MonsterTransformCom()->Get_State(CTransform::STATE_POSITION));
}

void CPlayer_BowAction::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CPlayer_BowAction::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);
}

void CPlayer_BowAction::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CPlayer_BowAction::State_Exit()
{
	m_pPlayer->Set_Attacking(false);

}

void CPlayer_BowAction::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);

	if (m_bAnimationFinished)
		Change_State_To_GetHitFront(pOther);
}

void CPlayer_BowAction::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CPlayer_BowAction::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
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
