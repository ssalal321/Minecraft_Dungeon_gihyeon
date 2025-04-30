#include "ArrowPool_Player.h"
#include "Player_Arrow.h"
#include "GameInstance.h"

CArrowPool_Player::CArrowPool_Player() : m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CArrowPool_Player::Initialize()
{
	for (_int i = 0; i < 3; ++i)
	{
		CGameObject* pArrowObject = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_PlayerArrow"),
													LEVEL_STATIC, TEXT("Layer_Arrow"));
		if (nullptr == pArrowObject)	return E_FAIL;

		CPlayer_Arrow*	 pArrow = dynamic_cast<CPlayer_Arrow*>(pArrowObject);
		m_PlayerArrowPool.push(pArrow);
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
	while (!m_PlayerArrowPool.empty())
	{
		CPlayer_Arrow* pArrow = m_PlayerArrowPool.front();
		m_PlayerArrowPool.pop();
		Safe_Release(pArrow);
	}
}

CPlayer_Arrow* CArrowPool_Player::Get_Arrow(_uint uiAttackPoint)
{
	if (m_PlayerArrowPool.empty())
	{
		// 풀에 없으면 새로 생성
		CGameObject* pArrowObject = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_PlayerArrow"),
			LEVEL_STATIC, TEXT("Layer_Arrow"));
		if (nullptr == pArrowObject)	return nullptr;

		CPlayer_Arrow* pNewArrow = dynamic_cast<CPlayer_Arrow*>(pArrowObject);
		m_PlayerArrowPool.push(pNewArrow);
		pNewArrow = m_PlayerArrowPool.front();
		m_PlayerArrowPool.pop();  // 맨 앞에 있는 걸 꺼내고 제거
		pNewArrow->Set_DealPoint(uiAttackPoint);

		return pNewArrow;
	}
	else
	{
		// 있으면 재사용
		CPlayer_Arrow*  pArrow = m_PlayerArrowPool.front();
		m_PlayerArrowPool.pop();
		pArrow->Set_DealPoint(uiAttackPoint);

		return pArrow;
	}
}

void CArrowPool_Player::Return_Arrow(CPlayer_Arrow* pArrow)
{
	if (pArrow)
	{
		pArrow->Reset(); // 재사용을 위해 상태 초기화
		m_PlayerArrowPool.push(pArrow);
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

	/*while (!m_PlayerArrowPool.empty())
	{
		CPlayer_Arrow* pArrow = m_PlayerArrowPool.front();
		Safe_Release(pArrow);
		m_PlayerArrowPool.pop();
	}*/

	Safe_Release(m_pGameInstance);
}
