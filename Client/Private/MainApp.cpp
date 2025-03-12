#include "../Public/MainApp.h"
#include "GameInstance.h"

#include "Level_Loading.h"

CMainApp::CMainApp()
    : m_pGameInstance { CGameInstance::GetInstance() }
{
     //D3D11_SAMPLER_DESC

    Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
    ENGINE_DESC         EngineDesc{};

    EngineDesc.hInstance = g_hInstance;
    EngineDesc.hWnd = g_hWnd;
    EngineDesc.isWindowed = true;
    EngineDesc.iViewportWidth = g_iWinSizeX;
    EngineDesc.iViewportHeight = g_iWinSizeY;
    EngineDesc.iNumLevels = LEVEL_END;

    if (FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext)))
        return E_FAIL;

    if (FAILED(Start_Level(LEVEL_LOGO)))
        return E_FAIL;

    return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
    m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
    m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
    m_pGameInstance->Clear_DepthStencil_View();

    m_pGameInstance->Draw();

    m_pGameInstance->Present();

    return S_OK;
}

HRESULT CMainApp::Start_Level(LEVEL eStartLevelID)
{
    if (nullptr == m_pGameInstance)
        return E_FAIL;

    if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevelID))))
        return E_FAIL;

    return S_OK;
}

CMainApp* CMainApp::Create()
{
    CMainApp* pGameInstance = new CMainApp();

    if (FAILED(pGameInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CMainApp");
        Safe_Release(pGameInstance);
    }

    return pGameInstance;
}

void CMainApp::Free()
{
    __super::Free();

    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);  
    
    m_pGameInstance->Release_Engine();
    
    Safe_Release(m_pGameInstance);    
}