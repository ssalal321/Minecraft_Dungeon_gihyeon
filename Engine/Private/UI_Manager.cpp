#include "UI_Manager.h"
#include "GameInstance.h"
#include "UIObject.h"


CUI_Manager::CUI_Manager() : m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
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

	CUIObject::UIOBJECT_DESC* uiObjectDesc = static_cast<CUIObject::tagUIObjectDesc*>(pArg);

	// UI 객체가 현재 레벨에 추가되는지, 아니면 다음 레벨에 추가되는지 구분
	if (iLayerLevelIndex != m_iObjectLevelIndex)
	{
		// 새로운 레벨로 추가 (다음 레벨 UI)
		m_NextUIObjects[eUILifeTime].insert({ uiObjectDesc->strGameObjectTag, pUIObject });
	}
	else 
	{
		// 현재 레벨 UI에 추가
		m_CurrentUIObjects[eUILifeTime].insert({ uiObjectDesc->strGameObjectTag, pUIObject });
	}

	return pUIObject;
}

CUIObject* CUI_Manager::Find_UIGameObject(_wstring strGameObjectTag, UI_LIFETIME eUILifeTime) const
{
	for (auto& uiObject : m_CurrentUIObjects[eUILifeTime])
	{
		if (uiObject.first == strGameObjectTag)
			return uiObject.second;
	}

	return nullptr;
}

HRESULT CUI_Manager::Delete_UIObject(const _wstring& strGameObjectTag, UI_LIFETIME eUILifeTime)
{
	if (eUILifeTime >= LIFETIME_END)
		return E_FAIL;

	auto& uiMap = m_CurrentUIObjects[eUILifeTime];

	auto iter = uiMap.find(strGameObjectTag);
	if (iter != uiMap.end())
	{
		Safe_Release(iter->second); // UIObject 메모리 해제
		uiMap.erase(iter);          // map에서 삭제
		return S_OK;
	}

	return E_FAIL; // 못 찾으면 실패
}


void CUI_Manager::Request_Add_UIObject(_uint iPrototypeLevelIndex, _uint iLayerLevelIndex, const _wstring& strPrototypeTag, UI_LIFETIME eUILifeTime, void* pArg)
{
	m_AddQueue.push_back({ iPrototypeLevelIndex, iLayerLevelIndex, strPrototypeTag, eUILifeTime, pArg });
}

void CUI_Manager::Process_AddQueue()
{
	for (auto& request : m_AddQueue)
	{
		Add_UIObject(request.iPrototypeLevelIndex, request.iLayerLevelIndex, request.strPrototypeTag, request.eUILifeTime, request.pArg);
	}
	m_AddQueue.clear();
}

void CUI_Manager::Request_Delete_UIObject(const _wstring& strGameObjectTag, UI_LIFETIME eUILifeTime)
{
	m_DeleteQueue.push_back({ strGameObjectTag, eUILifeTime });
}

void CUI_Manager::Process_DeleteQueue()
{
	for (auto& pair : m_DeleteQueue)
		Delete_UIObject(pair.first, pair.second);

	m_DeleteQueue.clear();
}


void CUI_Manager::Priority_Update(_float fDeltaTime)
{
	for (_uint i = 0; i < LIFETIME_END; ++i)
	{
		for (auto& uiObject : m_CurrentUIObjects[i])
			uiObject.second->Priority_Update(fDeltaTime);  // 모든 UI 객체 업데이트
	}
}

void CUI_Manager::Update(_float fDeltaTime)
{
	for (_uint i = 0; i < LIFETIME_END; ++i)
	{
		for (auto& uiObject : m_CurrentUIObjects[i])
			uiObject.second->Update(fDeltaTime);  // 모든 UI 객체 업데이트
	}

	Process_DeleteQueue();
	//Process_AddQueue();
}

void CUI_Manager::Late_Update(_float fDeltaTime)
{
	for (_uint i = 0; i < LIFETIME_END; ++i)
	{
		for (auto& uiObject : m_CurrentUIObjects[i])
			uiObject.second->Late_Update(fDeltaTime);  // 모든 UI 객체 업데이트
	}
}

HRESULT CUI_Manager::Render_UI()
{
	for (int i = 0; i < LIFETIME_END; ++i)
	{
		for (auto& uiObject : m_CurrentUIObjects[i])
		{
			uiObject.second->Render();
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
			Safe_Release(pUIObject.second);

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
	for (auto& uiMap : m_CurrentUIObjects)
	{
		for (auto& pair : uiMap)
		{
			Safe_Release(pair.second);  // 올바르게 UI 객체 해제
		}
		uiMap.clear();
	}

	// 다음 레벨 UI 객체들 해제
	for (auto& uiMap : m_NextUIObjects)
	{
		for (auto& pair : uiMap)
		{
			Safe_Release(pair.second);  // 올바르게 UI 객체 해제
		}
		uiMap.clear();
	}

	Safe_Release(m_pGameInstance);
}
