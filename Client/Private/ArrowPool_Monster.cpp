#include "ArrowPool_Monster.h"
#include "Monster_Arrow.h"
#include "GameInstance.h"

CArrowPool_Monster::CArrowPool_Monster() : m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CArrowPool_Monster::Initialize()
{
	for (_int i = 0; i < 3; ++i)
	{
		CGameObject* pArrowObject = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_MonsterArrow"),
													LEVEL_STATIC, TEXT("Layer_Arrow"));
		if (nullptr == pArrowObject)
			return E_FAIL;

		CMonster_Arrow*	 pArrow = dynamic_cast<CMonster_Arrow*>(pArrowObject);
		pArrow->Set_Pool(this);
		m_MonsterArrowPool.push(pArrow);
	}

	return S_OK;
}

void CArrowPool_Monster::Clear(_uint iCurrentLevelIndex, _uint iNextLevelIndex)
{
	while (!m_MonsterArrowPool.empty())
	{
		CMonster_Arrow* pArrow = m_MonsterArrowPool.front();
		m_MonsterArrowPool.pop();
		Safe_Release(pArrow);
	}
}

CMonster_Arrow* CArrowPool_Monster::Get_Arrow(_uint uiDealPoint)
{
	if (m_MonsterArrowPool.empty())
	{
		// 풀에 없으면 새로 생성
		CGameObject* pArrowObject = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_MonsterArrow"),
			LEVEL_STATIC, TEXT("Layer_Arrow"));
		if (nullptr == pArrowObject)	return nullptr;

		CMonster_Arrow* pNewArrow = dynamic_cast<CMonster_Arrow*>(pArrowObject);
		m_MonsterArrowPool.push(pNewArrow);
		pNewArrow->Set_Pool(this);

		pNewArrow = m_MonsterArrowPool.front();
		m_MonsterArrowPool.pop();  // 맨 앞에 있는 걸 꺼내고 제거
		pNewArrow->Set_DealPoint(uiDealPoint);

		return pNewArrow;
	}
	else
	{
		// 있으면 재사용
		CMonster_Arrow*  pArrow = m_MonsterArrowPool.front();
		m_MonsterArrowPool.pop();
		pArrow->Set_DealPoint(uiDealPoint);

		return pArrow;
	}
}

void CArrowPool_Monster::Return_Arrow(CMonster_Arrow* pArrow)
{
	if (false == pArrow->Get_GameObject_Active())
	{
		m_MonsterArrowPool.push(pArrow);
	}
}

CArrowPool_Monster* CArrowPool_Monster::Create()
{
	CArrowPool_Monster* pGameInstance = new CArrowPool_Monster();

	if (FAILED(pGameInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CArrowPool_Monster");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CArrowPool_Monster::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
