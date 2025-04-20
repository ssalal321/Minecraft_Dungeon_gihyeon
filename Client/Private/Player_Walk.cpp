#include "Player_Walk.h"
#include "Body_Player.h"

CPlayer_Walk::CPlayer_Walk(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEPLAYER_DESC* pDesc)
	: CState_Player(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CPlayer_Walk::Init_State()
{
	__super::Init_State();

	return S_OK;
}

void CPlayer_Walk::State_Enter()
{
	// ¹Ù²ã¾ßµÅ!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	m_pActorModelCom->Set_Animation(static_cast<_uint>(PLAYER_STATE::WALK_GLAIVE), true, 1.6f);
}

void CPlayer_Walk::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

// Player_Walk
void CPlayer_Walk::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (Change_State_To_Roll())
		return;

	if (Change_State_To_Attack())
		return;

	Chase_Monster();

	Change_State_To_Walk();	

	Walk_Through_Destination(fTimeDelta);

}

void CPlayer_Walk::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CPlayer_Walk::State_Exit()
{
}

void CPlayer_Walk::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);
}

void CPlayer_Walk::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CPlayer_Walk::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

void CPlayer_Walk::Walk_Through_Destination(_float fTimeDelta)
{
	_vector  vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector  vNextPos = XMLoadFloat4(&m_pPlayer->Get_NextPosition());
	//XMVectorSetY(vNextPos, 0.f);

	_vector  vToTarget = vNextPos - vCurPos;
	XMVectorSetY(vToTarget, 0.f);

	_float fDist = XMVectorGetX(XMVector3Length(vToTarget));

	if (fDist < 0.1f)
	{
		if (Change_State_To_Idle())
			return;
	}

	m_pTransformCom->LookAt(vNextPos);

	m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
}

void CPlayer_Walk::Chase_Monster()
{
	if (m_pPlayer->Get_Chasing())
	{
		CTransform* pMonsterTransformCom = m_pPlayer->Get_MonsterTransformCom();
		_float4 pMonsterPos = {};
		XMStoreFloat4(&pMonsterPos, pMonsterTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pPlayer->Set_NextPosition(pMonsterPos);
	}
}

CState_Player* CPlayer_Walk::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEPLAYER_DESC* pDesc)
{
	CPlayer_Walk* pGameInstance = new CPlayer_Walk(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CPlayer_Walk");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CPlayer_Walk::Free()
{
	__super::Free();
}
