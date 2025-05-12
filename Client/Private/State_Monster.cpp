#include "State_Monster.h"

#include <iostream>
#include <ostream>

#include "Player.h"

//_bool CState_Monster::m_bAnimationFinished = false;

CState_Monster::CState_Monster(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState(pActor, pGameObjectDesc), m_pStateMonsterDesc(pDesc)
{
}

HRESULT CState_Monster::Init_State()
{
	m_pMonsterInfo = dynamic_cast<CMonster::MONSTER_DESC*>(m_pGameObjectInfo);

	m_pActorModelCom	= m_pStateMonsterDesc->pActorModelCom;
	m_pBigColliderCom	= m_pStateMonsterDesc->pBigColliderCom;
	m_pSmallColliderCom = m_pStateMonsterDesc->pSmallColliderCom;
	m_pTransformCom		= m_pStateMonsterDesc->pTransformCom;
	m_pNavigationCom	= m_pStateMonsterDesc->pNavigationCom;

	if (nullptr == m_pMonsterInfo || nullptr == m_pActorModelCom || nullptr == m_pTransformCom ||
		/*nullptr == m_pNavigationCom ||*/ nullptr == m_pBigColliderCom)
		return E_FAIL;

	return S_OK;
}

void CState_Monster::State_Enter()
{
}

void CState_Monster::State_Priority_Update(_float fTimeDelta)
{
	// 체력 0이면 죽기
}

void CState_Monster::State_Update(_float fTimeDelta)
{
	if (m_pMonsterInfo->Get_CurrentHP() <= 0)
	{
		m_pActor->Set_GameObject_Active(false);
		m_pBigColliderCom->Set_ColliderActive(false);
		m_pSmallColliderCom->Set_ColliderActive(false);
	}

	m_bAnimationFinished = m_pActorModelCom->Play_Animation(fTimeDelta);
}

void CState_Monster::State_Late_Update(_float fTimeDelta)
{
}

void CState_Monster::State_Exit()
{
}

void CState_Monster::Collision_Enter(CCollider* pOther)
{
	if (/*pOther->Get_ColliderTag() == TEXT("Player_Body") ||*/
		pOther->Get_ColliderTag() == TEXT("Monster_Body_Small"))
	{
		CTransform* pTransformcom = dynamic_cast<CTransform*>(pOther->Get_OwnerObject()->Find_Component(TEXT("Com_Transform")));
		if (nullptr != pTransformcom)
		{
			_float4  otherPosition;
			XMStoreFloat4(&otherPosition, pTransformcom->Get_State(CTransform::STATE_POSITION));
			dynamic_cast<CMonster*>(m_pActor)->Resolve_Penetration_And_Slide(pOther, 0.2f);
		}
	}
}

void CState_Monster::Collision_Stay(CCollider* pOther)
{
	//if (/*pOther->Get_ColliderTag() == TEXT("Player_Body") ||*/
	//	pOther->Get_ColliderTag() == TEXT("Monster_Body_Small"))
	//{
	//	CTransform* pTransformcom = dynamic_cast<CTransform*>(pOther->Get_OwnerObject()->Find_Component(TEXT("Com_Transform")));
	//	if (nullptr != pTransformcom)
	//	{
	//		_float4  otherPosition;
	//		XMStoreFloat4(&otherPosition, pTransformcom->Get_State(CTransform::STATE_POSITION));
	//		dynamic_cast<CMonster*>(m_pActor)->Resolve_Penetration_And_Slide(pOther, 0.0001f);
	//	}
	//}
}

void CState_Monster::Collision_Exit(CCollider* pOther)
{
}

void CState_Monster::Free()
{
	__super::Free();
}
