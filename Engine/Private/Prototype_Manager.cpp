#include "Prototype_Manager.h"
#include "GameObject.h"
#include "Component.h"

CPrototype_Manager::CPrototype_Manager()
{
}

HRESULT CPrototype_Manager::Initialize(_uint iNumLevels)
{
	if (nullptr != m_pPrototypes)
		return E_FAIL;

	m_iNumLevels = iNumLevels;

	m_pPrototypes = new map<const _wstring, CBase*>[iNumLevels];

    return S_OK;
}

HRESULT CPrototype_Manager::Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype)
{
	if (nullptr == m_pPrototypes ||
		iLevelIndex >= m_iNumLevels ||
		nullptr != Find_Prototype(iLevelIndex, strPrototypeTag))
		return E_FAIL;

	m_pPrototypes[iLevelIndex].emplace(strPrototypeTag, pPrototype);

	return S_OK;
}

CBase* CPrototype_Manager::Clone_Prototype(PROTOTYPE ePrototype, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	/* 원형객체를 검색한다. */
	CBase* pPrototype = Find_Prototype(iPrototypeLevelIndex, strPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	/* 복제하여 사본을 생성한다. */
	CBase* pCloneObject = { nullptr };

	switch (ePrototype)
	{
	case PROTOTYPE::PROTOTYPE_GAMEOBJECT:
		pCloneObject = dynamic_cast<CGameObject*>(pPrototype)->Clone(pArg);
		break;

	case PROTOTYPE::PROTOTYPE_COMPONENT:
		pCloneObject = dynamic_cast<CComponent*>(pPrototype)->Clone(pArg);
		break;
	}

	if (nullptr == pCloneObject)
		return nullptr;

	return pCloneObject;
}

void CPrototype_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return;

	for (auto& Pair : m_pPrototypes[iLevelIndex])	
		Safe_Release(Pair.second);

	m_pPrototypes[iLevelIndex].clear();
}

CBase* CPrototype_Manager::Find_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag)
{
	auto	iter = m_pPrototypes[iLevelIndex].find(strPrototypeTag);
	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

CPrototype_Manager* CPrototype_Manager::Create(_uint iNumLevels)
{
	CPrototype_Manager* pInstance = new CPrototype_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Create : CPrototype_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPrototype_Manager::Free()
{
    __super::Free();

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pPrototypes[i])
			Safe_Release(Pair.second);

		m_pPrototypes[i].clear();
	}

	Safe_Delete_Array(m_pPrototypes);
}
