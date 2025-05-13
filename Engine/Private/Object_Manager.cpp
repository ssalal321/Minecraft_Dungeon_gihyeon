#include "Object_Manager.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Layer.h"

CObject_Manager::CObject_Manager()
    : m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

CGameObject* CObject_Manager::Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	if (nullptr == m_pLayers || 
		iLayerLevelIndex >= m_iNumLevels)
		return nullptr;

	CGameObject*	pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTOTYPE_GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, pArg));
	if (nullptr == pGameObject)
		return nullptr;

	return Add_To_Layer(pGameObject, iLayerLevelIndex, strLayerTag, pGameObject->Get_GameObjectTag());
}

HRESULT CObject_Manager::Initialize(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_iNumLevels = iNumLevels;

	m_pLayers			= new map<const _wstring, CLayer*>[iNumLevels];
	m_pPersistentLayers = new map<const _wstring, CLayer*>;

	return S_OK;
}

void CObject_Manager::Priority_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->Priority_Update(fTimeDelta);		
		}
	}
}

void CObject_Manager::Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Update(fTimeDelta);
	}
}

void CObject_Manager::Late_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Late_Update(fTimeDelta);
	}
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels || nullptr == m_pLayers)
		return;

	for (auto& pair : m_pLayers[iLevelIndex])
	{
		const wstring& strLayerTag = pair.first;

		// Persistent Layer라면 스킵
		if (m_pPersistentLayers->find(strLayerTag) != m_pPersistentLayers->end())
			continue;

		Safe_Release(pair.second);
	}

	// 다시 초기화 (persistent layer만 남기고 clear)
	m_pLayers[iLevelIndex].clear();

	/*for (const auto& pair : *m_pPersistentLayers)
	{
		m_pLayers[iLevelIndex].emplace(pair.first, pair.second);
	}*/
}

CGameObject* CObject_Manager::Find_GameObject(const _wstring& strGameObjectTag, _uint iLayerLevelIndex,
	const _wstring& strLayerTag)
{
	if (nullptr == m_pLayers || iLayerLevelIndex >= m_iNumLevels)
		return nullptr;

	CLayer* pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Find_GameObject(strGameObjectTag);
}

CLayer* CObject_Manager::Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag)
{
	auto	iter = m_pLayers[iLevelIndex].find(strLayerTag);

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

HRESULT CObject_Manager::Set_Layer_Persistent(_uint iLevelIndex, const wstring& strLayerTag)
{
	if (iLevelIndex >= m_iNumLevels || nullptr == m_pLayers || nullptr == m_pPersistentLayers)
		return E_FAIL;

	auto& layerMap = m_pLayers[iLevelIndex];
	auto iter = layerMap.find(strLayerTag);
	if (iter == layerMap.end())
		return E_FAIL;

	// 이미 등록된 경우 중복 방지
	if (m_pPersistentLayers->find(strLayerTag) != m_pPersistentLayers->end())
		return S_OK;

	m_pPersistentLayers->emplace(strLayerTag, iter->second);

	return S_OK;
}

CLayer* CObject_Manager::Get_Persistent_Layer(const wstring& strLayerTag)
{
	if (!m_pPersistentLayers)
		return nullptr;

	auto iter = m_pPersistentLayers->find(strLayerTag);
	if (iter == m_pPersistentLayers->end())
		return nullptr;

	return iter->second;
}

// m_pPersistentLayers에 저장되어 있더라도 m_pLayers에 다시 넣어줘야 update가 된다.
// m_pPersistentLayers에 저장된 layer 중 원하는 레이어만 넣어주는 용
// Persistent Layer를 명시적으로 현재 Level에 등록하는 함수
HRESULT CObject_Manager::Attach_Persistent_Layer_To_Level(_uint iTargetLevelIndex, const wstring& strLayerTag)
{
	if (iTargetLevelIndex >= m_iNumLevels || nullptr == m_pLayers || nullptr == m_pPersistentLayers)
		return E_FAIL;

	auto persistentIter = m_pPersistentLayers->find(strLayerTag);
	if (persistentIter == m_pPersistentLayers->end())
		return E_FAIL;

	auto& layerMap = m_pLayers[iTargetLevelIndex];

	// 이미 해당 레이어가 존재하는 경우는 덮어쓰지 않음
	if (layerMap.find(strLayerTag) != layerMap.end())
		return S_OK;

	layerMap.emplace(strLayerTag, persistentIter->second);

	return S_OK;
}


// m_pPersistentLayers에 저장된 layer들을 전부 현재 m_pLayer에 넣어주는 용
HRESULT CObject_Manager::Attach_Persistent_Layers_To_Level(_uint iLevelIndex)  
{
	if (iLevelIndex >= m_iNumLevels || nullptr == m_pLayers || nullptr == m_pPersistentLayers)
		return E_FAIL;

	for (const auto& pair : *m_pPersistentLayers)
	{
		auto& layerMap = m_pLayers[iLevelIndex];

		if (layerMap.find(pair.first) == layerMap.end())
			layerMap.emplace(pair.first, pair.second);
	}

	return S_OK;
}

void CObject_Manager::Set_Object_Backup_Data(void* pObjectData1, void* pObjectData2)
{
	m_tObjectDataBackup.pObjectData1 = pObjectData1;
	m_tObjectDataBackup.pObjectData2 = pObjectData2;
}


CGameObject* CObject_Manager::Add_To_Layer(CGameObject* pGameObject, _uint iLayerLevelIndex, const _wstring& strLayerTag, const _wstring& strGameObjectTag)
{
	CLayer* pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return nullptr;

		if (FAILED(pLayer->Add_GameObject(strGameObjectTag, pGameObject)))
			return nullptr;

		m_pLayers[iLayerLevelIndex].emplace(strLayerTag, pLayer);
	}
	else
		if (FAILED(pLayer->Add_GameObject(strGameObjectTag, pGameObject)))
			return nullptr;

	return pGameObject;
}


CObject_Manager* CObject_Manager::Create(_uint iNumLevels)
{
	CObject_Manager* pInstance = new CObject_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CObject_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CObject_Manager::Free()
{
    __super::Free();

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);

		m_pLayers[i].clear();
	}

	if (m_pPersistentLayers)
	{
		for (auto& pair : *m_pPersistentLayers)
			Safe_Release(pair.second);

		m_pPersistentLayers->clear();
	}

	Safe_Delete_Array(m_pLayers);
	Safe_Delete(m_pPersistentLayers);

    Safe_Release(m_pGameInstance);
}
