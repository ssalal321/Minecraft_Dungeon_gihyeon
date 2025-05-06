#include "Level_Loading.h"
#include "GameInstance.h"
#include "Loader.h"

#include "UI_Image.h"
#include "Level_Lounge.h"
#include "Level_SoggySwamp.h"
#include "Level_Title.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
    m_eNextLevelID = eNextLevelID;

	m_pGameInstance->Set_NextLevelIndex(m_eNextLevelID);

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
    if (true == m_pLoader->Is_Finished() && m_pGameInstance->Key_Down(VK_RETURN))
    {
        CLevel* pNewLevel = { nullptr };

        switch (m_eNextLevelID)
        {
        case LEVEL_TITLE:
            pNewLevel = CLevel_Title::Create(m_pDevice, m_pContext);
            break;


        case LEVEL_LOUNGE:
            pNewLevel = CLevel_Lounge::Create(m_pDevice, m_pContext);
            break;

        case LEVEL_SOGGYSWAMP:
            pNewLevel = CLevel_SoggySwamp::Create(m_pDevice, m_pContext);
            break;
        }

        if (nullptr == pNewLevel)
            return;

        if (SUCCEEDED(m_pGameInstance->Open_Level(m_eNextLevelID, pNewLevel)))
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
    /* For.Prototype_Component_Shader_VtxPosTex */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_Component_Shader_VtxPosTex"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
        return E_FAIL;

    /* For.Prototype_Component_VIBuffer_Rect */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_Component_VIBuffer_Rect"),
        CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_Component_Texture_LoadingScreen */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_Component_Texture_LoungeLoading"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Loading/Loading_Screen_Lobby%d.png"), 3))))
        return E_FAIL; 

    /* For.Prototype_GameObject_UIImage */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_GameObject_UIImage"),
        CUI_Image::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    CUI_Image::UIIMAGE_DESC  LoungeLoadingDesc
    (TEXT("GameObject_LoungeLoading"), CUI_Image::UNCLICKABLE, 
        g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f, 0.9f, g_iWinSizeX, g_iWinSizeY,
        L"Prototype_Component_Texture_LoungeLoading", LEVEL_LOADING, LEVEL_LOADING, 180.f);

    CUIObject* pLoungeLoading = m_pGameInstance->Add_UIObject(LEVEL_LOADING, LEVEL_LOADING,
        TEXT("Prototype_GameObject_UIImage"),
        CUI_Manager::TEMPORARY, &LoungeLoadingDesc);

    if (nullptr == pLoungeLoading) return E_FAIL;

    return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
    CLevel_Loading* pGameInstance = new CLevel_Loading(pDevice, pContext);

    if (FAILED(pGameInstance->Initialize(eNextLevelID)))
    {
        MSG_BOX("Failed to Create : CLevel_Loading");
        Safe_Release(pGameInstance);
    }

    return pGameInstance;
}


void CLevel_Loading::Free()
{
    __super::Free();

    Safe_Release(m_pLoader);
}
