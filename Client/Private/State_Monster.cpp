#include "State_Monster.h"

#include <iostream>
#include <ostream>

//_bool CState_Monster::m_bAnimationFinished = false;

CState_Monster::CState_Monster(CGameObject* pActor, CModel* pMonsterModelCom, CCollider* pColliderCom,
								CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
								CTransform* pTransformCom, CNavigation* pNavigationCom)
	: CState(pActor, pMonsterModelCom, pColliderCom, pGameObjectDesc, pTransformCom, pNavigationCom)
{
}

HRESULT CState_Monster::Init_State()
{
	m_pMonsterDesc = dynamic_cast<CMonster::MONSTER_DESC*>(m_pGameObjectDesc);

	if (nullptr == m_pMonsterDesc)
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
}

void CState_Monster::Collision_Stay(CCollider* pOther)
{
}

void CState_Monster::Collision_Exit(CCollider* pOther)
{
}

void CState_Monster::Free()
{
	__super::Free();
}
