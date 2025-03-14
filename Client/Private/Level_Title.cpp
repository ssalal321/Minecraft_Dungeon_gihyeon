#include "Level_Title.h"
#include "GameInstance.h"

#include "Level_Loading.h"
#include "UI_Image.h"

CLevel_Title::CLevel_Title(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Title::Initialize()
{
    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_TitleScreen"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_Title::Update(_float fTimeDelta)
{
    if (GetKeyState(VK_SPACE) & 0x8000)
    {
        if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING,
            CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
            return;
    }
}

HRESULT CLevel_Title::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("타이틀입니다."));
#endif

    return S_OK;
}

HRESULT CLevel_Title::Ready_Layer_BackGround(const _wstring& strLayerTag)
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
    Desc.strTextureComTag       = TEXT("Prototype_Component_Texture_TitleImage");*/

    CUI_Image::UIIMAGE_DESC  UIImageDesc
	(TEXT("GameObject_TitleImage"), CUI_Image::UNCLICKABLE, LEVEL_STATIC, LEVEL_TITLE,
	 g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f, g_iWinSizeX, g_iWinSizeY,
	 3.0f, L"Prototype_Component_Texture_TitleImage", 5.0f, 180.0f);


    if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC,
											   TEXT("Prototype_GameObject_UIImage"),
											   LEVEL_TITLE, strLayerTag, &UIImageDesc)))
        return E_FAIL;

    return S_OK;
}

CLevel_Title* CLevel_Title::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Title* pGameInstance = new CLevel_Title(pDevice, pContext);

    if (FAILED(pGameInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_Title");
        Safe_Release(pGameInstance);
    }

    return pGameInstance;
}


void CLevel_Title::Free()
{
    __super::Free();

}
