#include "Level_ImGui.h"
#include "GameInstance.h"

#include "Level_Loading.h"
#include "UI_Image.h"

CLevel_ImGui::CLevel_ImGui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel { pDevice, pContext }
{
}

HRESULT CLevel_ImGui::Initialize()
{
    if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_ImGui::Update(_float fTimeDelta)
{
    /*if (m_pGameInstance->Key_Down(VK_RETURN))
    {
        if (SUCCEEDED(m_pGameInstance->Open_Level(LEVEL_LOADING,
            CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_LOUNGE))))
            return;
    }*/
}

HRESULT CLevel_ImGui::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("타이틀입니다."));
#endif

    return S_OK;
}

HRESULT CLevel_ImGui::Ready_Layer_UI(const _wstring& strLayerTag)
{
    /*CUI_Image::UIIMAGE_DESC   Desc{};

    Desc.pGameObjectTag     = TEXT("GameObject_TitleScreen");
    Desc.fSpeedPerSec       = 5.f;
    Desc.fRotationPerSec    = XMConvertToRadians(180.f);

    Desc.iPrototypeLevelIndex   = LEVEL_STATIC;
    Desc.iLayerLevelIndex       = LEVEL_TITLE;
    Desc.fX                     = g_iWinSizeX * 0.5f;
    Desc.fY                     = g_iWinSizeY * 0.5f;
    Desc.fSizeX                 = g_iWinSizeX;
    Desc.fSizeY                 = g_iWinSizeY;
    Desc.fPlayTime              = 3.f;
    Desc.strTexPrototypeTag       = TEXT("Prototype_Component_Texture_TitleImage");*/


    return S_OK;
}

CLevel_ImGui* CLevel_ImGui::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_ImGui* pGameInstance = new CLevel_ImGui(pDevice, pContext);

    if (FAILED(pGameInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_ImGui");
        Safe_Release(pGameInstance);
    }

    return pGameInstance;
}


void CLevel_ImGui::Free()
{
    __super::Free();

}
