#include "MainApp.h"
#include "GameInstance.h"

#include "Level_Loading.h"

CMainApp::CMainApp()
    : m_pGameInstance { CGameInstance::GetInstance() }
{
    /*XMMatrixDecompose();*/
    Safe_AddRef(m_pGameInstance);

    /*m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, FALSE);*/

    //ID3D11RasterizerState*;
    //D3D11_RASTERIZER_DESC;
    //m_pDevice->CreateRasterizerState();
    //m_pContext->RSSetState();

    //ID3D11DepthStencilState*;
    //D3D11_DEPTH_STENCIL_DESC;
    //m_pDevice->CreateDepthStencilState();
    //m_pContext->OMSetDepthStencilState();

    //ID3D11BlendState*;
    //D3D11_BLEND_DESC;
    //m_pDevice->CreateBlendState();
    //m_pContext->OMSetBlendState();
}

HRESULT CMainApp::Initialize()
{
    ENGINE_DESC         EngineDesc{};

    EngineDesc.hInstance        = g_hInstance;
    EngineDesc.hWnd             = g_hWnd;
    EngineDesc.isWindowed       = true;
    EngineDesc.iViewportWidth   = g_iWinSizeX;
    EngineDesc.iViewportHeight  = g_iWinSizeY;
    EngineDesc.iNumLevels       = LEVEL_END;

    if (FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext)))
        return E_FAIL;

    if (FAILED(Start_Level(LEVEL_TITLE)))
        return E_FAIL;

    return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
    m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
    m_pGameInstance->Clear_BackBuffer_View(_float4(0.2f, 0.0f, 0.3f, 1.0f));
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