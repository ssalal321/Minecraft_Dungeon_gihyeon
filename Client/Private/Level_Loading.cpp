#include "Level_Loading.h"

#include "Loader.h"
#include "Level_Loading.h"
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

    /* 로딩레벨을 구성해주기위한 객체들을 생성한다. */
    if (FAILED(Ready_Layer_BackGround()))
        return E_FAIL;

    /* 다음레벨을 위한 자원을 준비한다. */
    m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
    if (nullptr == m_pLoader)
        return E_FAIL;

    return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
    if (true == m_pLoader->isFinished()/* && 
        GetKeyState(VK_SPACE) & 0x8000*/)
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
    /*CUI_Image::UIIMAGE_DESC   Desc{};

    Desc.fPlayTime = 3.f;
    Desc.pGameObjectTag = TEXT("GameObject_LoadingScreen");
    Desc.fSpeedPerSec = 5.f;
    Desc.fRotationPerSec = XMConvertToRadians(180.f);


    if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_LOADING, TEXT("Prototype_GameObject_TitleBase"),
        LEVEL_LOADING, strLayerTag, &Desc)))
        return E_FAIL;*/

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
