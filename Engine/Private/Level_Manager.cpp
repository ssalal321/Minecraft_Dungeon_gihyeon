#include "Level_Manager.h"

#include "GameInstance.h"
#include "Level.h"

CLevel_Manager::CLevel_Manager()
    : m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CLevel_Manager::Initialize()
{
    return S_OK;
}

HRESULT CLevel_Manager::Open_Level(_uint iNextLevelIndex, class CLevel* pNewLevel)
{
    if (nullptr == pNewLevel)
        return E_FAIL;

    if (nullptr != m_pCurrentLevel)
    {
        /* 기존 레벨용 자원(CPrototypeManager, CObjectManager) 을 파괴한다.  */
        m_pGameInstance->Clear(m_iLevelIndex, iNextLevelIndex);
    }

    if (0 != Safe_Release(m_pCurrentLevel))
        return E_FAIL;        

    m_pCurrentLevel = pNewLevel;

    m_iLevelIndex = iNextLevelIndex;

    return S_OK;
}

void CLevel_Manager::Update(_float fTimeDelta)
{
    if (nullptr != m_pCurrentLevel)
        m_pCurrentLevel->Update(fTimeDelta);
}

HRESULT CLevel_Manager::Render()
{
    if (nullptr != m_pCurrentLevel)
        return m_pCurrentLevel->Render();

    return S_OK;
}

CLevel_Manager* CLevel_Manager::Create()
{
    CLevel_Manager* pGameInstance = new CLevel_Manager();

    if (FAILED(pGameInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_Manager");
        Safe_Release(pGameInstance);
    }

    return pGameInstance;
}

void CLevel_Manager::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pCurrentLevel);
}
