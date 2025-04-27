#include "ArrowPool_Player.h"
#include "Arrow.h"
#include "GameInstance.h"

CArrowPool_Player::CArrowPool_Player() : m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CArrowPool_Player::Initialize()
{
	for (int i = 0; i < 30; ++i)
	{
		CGameObject* pArrowObject = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Arrow"),
													LEVEL_STATIC, TEXT("Layer_Arrow"));
		if (nullptr == pArrowObject)	return E_FAIL;

		CArrow*	 pArrow = dynamic_cast<CArrow*>(pArrowObject);
		m_ArrowPool.push(pArrow);
	}

	return S_OK;
}

//void CArrowPool_Player::Update(_float fTimeDelta)
//{
//}
//
//HRESULT CArrowPool_Player::Draw()
//{
//
//	return S_OK;
//}

void CArrowPool_Player::Clear(_uint iCurrentLevelIndex, _uint iNextLevelIndex)
{
	while (!m_ArrowPool.empty())
	{
		CArrow* pArrow = m_ArrowPool.front();
		m_ArrowPool.pop();
		Safe_Release(pArrow);
	}
}

CArrow* CArrowPool_Player::Get_Arrow()
{
	if (m_ArrowPool.empty())
	{
		// 풀에 없으면 새로 생성
		CGameObject* pArrowObject = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Arrow"),
			LEVEL_STATIC, TEXT("Layer_Arrow"));
		if (nullptr == pArrowObject)	return nullptr;

		CArrow* pNewArrow = dynamic_cast<CArrow*>(pArrowObject);
		m_ArrowPool.push(pNewArrow);
		pNewArrow = m_ArrowPool.front();
		m_ArrowPool.pop();  // 맨 앞에 있는 걸 꺼내고 제거

		return pNewArrow;
	}
	else
	{
		// 있으면 재사용
		CArrow*  pArrow = m_ArrowPool.front();
		m_ArrowPool.pop();

		return pArrow;
	}
}

void CArrowPool_Player::Return_Arrow(CArrow* pArrow)
{
	if (pArrow)
	{
		pArrow->Reset(); // 재사용을 위해 상태 초기화
		m_ArrowPool.push(pArrow);
	}
}

CArrowPool_Player* CArrowPool_Player::Create()
{
	CArrowPool_Player* pGameInstance = new CArrowPool_Player();

	if (FAILED(pGameInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CArrowPool_Player");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CArrowPool_Player::Free()
{
	__super::Free();

	while (!m_ArrowPool.empty())
	{
		CArrow* pArrow = m_ArrowPool.front();
		Safe_Release(pArrow);
		m_ArrowPool.pop();
	}

	Safe_Release(m_pGameInstance);
}
