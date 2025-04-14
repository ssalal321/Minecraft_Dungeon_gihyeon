#include "State_Monster.h"

_bool CState_Monster::m_bAnimationFinished = false;

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

//CState* CState_Player::Create(CGameObject* pActor, CGameObject* pPartObject, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
//							  CTransform* pTransformCom, CNavigation* pNavigationCom)
//{
//	CState* pGameInstance = new CState_Player(pActor, pPartObject, pGameObjectDesc, pTransformCom, pNavigationCom);
//
//	if (FAILED(pGameInstance->Init_State()))
//	{
//		MSG_BOX("Failed to Create : CState_Player");
//		Safe_Release(pGameInstance);
//	}
//
//	return pGameInstance;
//}

void CState_Monster::Free()
{
	__super::Free();
}
