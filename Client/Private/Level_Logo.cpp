#include "Level_Logo.h"
#include "GameInstance.h"

#include "Level_Loading.h"
#include "BackGround.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Logo::Initialize()
{
    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
    if (GetKeyState(VK_RETURN) & 0x8000)
    {
        if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING,
            CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
            return;
    }
}

HRESULT CLevel_Logo::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("로고레벨입니다."));
#endif

    return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
    CBackGround::BACKGROUND_DESC        Desc{};

    Desc.fPlayTime = 3.f;
    Desc.pGameObjectTag = TEXT("GameObject_BackGround");
    Desc.fSpeedPerSec = 5.f;
    Desc.fRotationPerSec = XMConvertToRadians(180.f);
    


    if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("Prototype_GameObject_BackGround"),
        LEVEL_LOGO, strLayerTag, &Desc)))
        return E_FAIL;

    return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Logo* pGameInstance = new CLevel_Logo(pDevice, pContext);

    if (FAILED(pGameInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_Logo");
        Safe_Release(pGameInstance);
    }

    return pGameInstance;
}


void CLevel_Logo::Free()
{
    __super::Free();

}
