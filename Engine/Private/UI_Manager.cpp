#include "UI_Manager.h"
#include "GameInstance.h"
#include "UIObject.h"


CUI_Manager::CUI_Manager() : m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);

	ZeroMemory(&m_CurrentUIObjects, NULL);
	ZeroMemory(&m_NextUIObjects, NULL);
}

HRESULT CUI_Manager::Initialize(_uint iNumLevels)
{
	m_iNumLevels = iNumLevels;

	return S_OK;
}


CUIObject* CUI_Manager::Add_UIObject(_uint iPrototypeLevelIndex, _uint iLayerLevelIndex, const _wstring& strPrototypeTag, UI_LIFETIME eUILifeTime, void* pArg)
{
	if (eUILifeTime >= LIFETIME_END)
		return nullptr;

	CUIObject*	pUIObject = dynamic_cast<CUIObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTOTYPE_GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, pArg));
	if (nullptr == pUIObject)
		return nullptr;

	// UI 객체가 현재 레벨에 추가되는지, 아니면 다음 레벨에 추가되는지 구분
	if (iLayerLevelIndex != m_iObjectLevelIndex)
	{
		// 새로운 레벨로 추가 (다음 레벨 UI)
		m_NextUIObjects[eUILifeTime].push_back(pUIObject);
	}
	else 
	{
		// 현재 레벨 UI에 추가
		m_CurrentUIObjects[eUILifeTime].push_back(pUIObject);
	}

	return pUIObject;
}

void CUI_Manager::Priority_Update(_float fDeltaTime)
{
	for (_uint i = 0; i < LIFETIME_END; ++i)
	{
		for (auto& uiObject : m_CurrentUIObjects[i])
			uiObject->Priority_Update(fDeltaTime);  // 모든 UI 객체 업데이트
	}
}

void CUI_Manager::Update(_float fDeltaTime)
{
	for (_uint i = 0; i < LIFETIME_END; ++i)
	{
		for (auto& uiObject : m_CurrentUIObjects[i])
			uiObject->Update(fDeltaTime);  // 모든 UI 객체 업데이트
	}
}

void CUI_Manager::Late_Update(_float fDeltaTime)
{
	for (_uint i = 0; i < LIFETIME_END; ++i)
	{
		for (auto& uiObject : m_CurrentUIObjects[i])
			uiObject->Late_Update(fDeltaTime);  // 모든 UI 객체 업데이트
	}
}

HRESULT CUI_Manager::Render_UI()
{
	for (int i = 0; i < LIFETIME_END; ++i)
	{
		for (auto& pUIObject : m_CurrentUIObjects[i])
		{
			pUIObject->Render();
		}
	}

	return S_OK;
}

void CUI_Manager::Clear(_uint iCurrentLevelIndex, _uint iNextLevelIndex)
{
	if (iCurrentLevelIndex >= m_iNumLevels)
		return;
	

	// 레벨 변경을 위한 UI 객체 교체
	if (iNextLevelIndex != m_iObjectLevelIndex)
	{
		// 현재 레벨 UI 삭제
		for (auto& pUIObject : m_CurrentUIObjects[TEMPORARY])
			Safe_Release(pUIObject);
		m_CurrentUIObjects[TEMPORARY].clear();

		// 다음 레벨 UI를 현재 레벨 UI로 이동
		std::swap(m_CurrentUIObjects, m_NextUIObjects);

		m_iObjectLevelIndex = iNextLevelIndex;  // 현재 레벨 인덱스 업데이트
	}
}

CUI_Manager* CUI_Manager::Create(_uint iNumLevels)
{
	CUI_Manager* pInstance = new CUI_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CUI_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Manager::Free()
{
	__super::Free();

	// 현재 레벨 UI 객체들 해제
	for (auto& uiVector : m_CurrentUIObjects)
	{
		for (auto& pUIObject : uiVector)
		{
			Safe_Release(pUIObject);  // UI 객체 해제
		}
		uiVector.clear();  // 벡터 초기화
	}

	// 다음 레벨 UI 객체들 해제
	for (auto& uiVector : m_NextUIObjects)
	{
		for (auto& pUIObject : uiVector)
		{
			Safe_Release(pUIObject);  // UI 객체 해제
		}
		uiVector.clear();  // 벡터 초기화
	}

	Safe_Release(m_pGameInstance);
}
