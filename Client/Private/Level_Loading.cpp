#include "Level_Loading.h"

#include "Loader.h"
#include <UI_Image.h>

#include "Level_Logo.h"
#include "Level_GamePlay.h"

#include "GameInstance.h"
#include "Level_Title.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
    m_eNextLevelID = eNextLevelID;

    /* 다음레벨을 위한 자원을 준비한다. */
    m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
    if (nullptr == m_pLoader)
        return E_FAIL;

    /* 로딩레벨을 구성해주기위한 객체들을 생성한다. */
    if (FAILED(Ready_Layer_BackGround()))
        return E_FAIL;

    return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
    if (true == m_pLoader->isFinished() && m_pGameInstance->Key_Down(VK_SPACE))
    {
        CLevel* pNewLevel = { nullptr };

        switch (m_eNextLevelID)
        {
        case LEVEL_LOGO:
            pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
            break;

        case LEVEL_GAMEPLAY:
            pNewLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
            break;

        case LEVEL_TITLE:
            pNewLevel = CLevel_Title::Create(m_pDevice, m_pContext);
            break;

        /*case LEVEL_LOUNGE:
            pNewLevel = CLevel_Lounge::Create(m_pDevice, m_pContext);
            break;*/
        }

        if (nullptr == pNewLevel)
            return;

        if (FAILED(m_pGameInstance->Open_Level(m_eNextLevelID, pNewLevel)))
            return;

        return;
    }
}

HRESULT CLevel_Loading::Render()
{
    if (nullptr == m_pLoader)
        return E_FAIL;

    m_pLoader->Show_LoadingText();

    return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround()
{
    /* For.Prototype_Component_Texture_LoadingScreen */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_Component_Texture_LoungeLoading"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Loading/Loading_Screen_Lobby%d.png"), 3))))
        return E_FAIL;

    CUI_Image::UIIMAGE_DESC  UIImageDesc
    (TEXT("GameObject_LoungeLoading"), CUI_Image::UNCLICKABLE, LEVEL_STATIC, LEVEL_LOADING,
        g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f, g_iWinSizeX, g_iWinSizeY,
        3.0f, L"Prototype_Component_Texture_LoungeLoading");

    if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC,
        TEXT("Prototype_GameObject_UIImage"),
        LEVEL_LOADING, TEXT("Layer_LoungeLoading"), &UIImageDesc)))
        return E_FAIL;

    return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
    CLevel_Loading* pGameInstance = new CLevel_Loading(pDevice, pContext);

    if (FAILED(pGameInstance->Initialize(eNextLevelID)))
    {
        MSG_BOX("Failed to Created : CLevel_Loading");
        Safe_Release(pGameInstance);
    }

    return pGameInstance;
}


void CLevel_Loading::Free()
{
    __super::Free();

    Safe_Release(m_pLoader);
}
