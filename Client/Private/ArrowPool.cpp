#include "ArrowPool.h"
#include "Arrow.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CArrowPool)

CArrowPool::CArrowPool() : m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CArrowPool::Initialize()
{
	for (int i = 0; i < 30; ++i)
	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Arrow"),
												 LEVEL_STATIC, TEXT("Layer_Arrow"))))
			return E_FAIL;

		CArrow* pArrow = dynamic_cast<CArrow*>
						(m_pGameInstance->Find_GameObject(TEXT("GameObject_Arrow"), LEVEL_STATIC, TEXT("Layer_Arrow")));


		m_ArrowPool.push(pArrow);
	}

	return S_OK;
}

void CArrowPool::Update(_float fTimeDelta)
{
}

HRESULT CArrowPool::Draw()
{

	return S_OK;
}

void CArrowPool::Clear(_uint iCurrentLevelIndex, _uint iNextLevelIndex)
{
	while (!m_ArrowPool.empty())
	{
		CArrow* pArrow = m_ArrowPool.front();
		m_ArrowPool.pop();
		Safe_Release(pArrow);
	}
}

void CArrowPool::Release_ArrowPool()
{
	DestroyInstance();
}

CArrow* CArrowPool::Get_Arrow()
{
	if (m_ArrowPool.empty())
	{
		// 풀에 없으면 새로 생성
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Arrow"),
			LEVEL_STATIC, TEXT("Layer_Arrow"))))
			return nullptr;

		CArrow* pArrow = dynamic_cast<CArrow*>
			(m_pGameInstance->Find_GameObject(TEXT("GameObject_Arrow"), LEVEL_STATIC, TEXT("Layer_Arrow")));

		return nullptr;   // 임시
	}
	else
	{
		// 있으면 재사용
		CArrow* pArrow = m_ArrowPool.front();
		m_ArrowPool.pop();
		return pArrow;
	}
}

void CArrowPool::Return_Arrow(CArrow* pArrow)
{
}

void CArrowPool::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
