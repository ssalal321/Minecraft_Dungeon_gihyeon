#include "State_Monster.h"

#include <iostream>
#include <ostream>

//_bool CState_Monster::m_bAnimationFinished = false;

CState_Monster::CState_Monster(CGameObject* pActor, CModel* pMonsterModelCom, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
							 CTransform* pTransformCom, CNavigation* pNavigationCom)
	: CState(pActor, pMonsterModelCom, pGameObjectDesc, pTransformCom, pNavigationCom)
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
	// 체력 0이면 죽엇
}

void CState_Monster::State_Update(_float fTimeDelta)
{
	m_bAnimationFinished = m_pActorModelCom->Play_Animation(fTimeDelta);

	if (m_bAnimationFinished)
	{
		std::cerr << "몬스터 애니메이션 루프 돌았다" << std::endl;
	}
}

void CState_Monster::State_Late_Update(_float fTimeDelta)
{
	
}

void CState_Monster::State_Exit()
{
}

void CState_Monster::Free()
{
	__super::Free();
}
