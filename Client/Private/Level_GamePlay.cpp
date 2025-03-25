#include "Level_GamePlay.h"

#include <UI_Image.h>

#include "GameInstance.h"

#include "Level_Loading.h"
#include "Camera_Free.h"
#include "../Default/HP_Bar.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel { pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize()
{
    if (FAILED(Ready_Lights()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
   
}

HRESULT CLevel_GamePlay::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
#endif

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
    LIGHT_DESC          LightDesc{};

    LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

    if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
        return E_FAIL;

    /*LightDesc.eType = LIGHT_DESC::TYPE_POINT;
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
    LightDesc.vPosition = _float4(20.f, 5.f, 20.f, 1.f);
    LightDesc.fRange = 20.f;
    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

    if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
        return E_FAIL;*/

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _wstring& strLayerTag)
{
    CCamera_Free::CAMERA_FREE_DESC            Desc{};

    Desc.pGameObjectTag = TEXT("GameObject_Camera_Free");
    Desc.vEye = _float3(0.f, 20.f, -15.f);
    Desc.vAt = _float3(0.f, 0.f, 0.f);
    Desc.fFov = XMConvertToRadians(60.f);
    Desc.fNear = 0.01f;
    Desc.fFar = 500.f;
    Desc.fMouseSensor = 0.07f;
    Desc.fSpeedPerSec = 10.f;
    Desc.fRotationPerSec = XMConvertToRadians(180.f);

    if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Camera_Free"),
        LEVEL_GAMEPLAY, strLayerTag, &Desc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _wstring& strLayerTag)
{
    /*for (size_t i = 0; i < 10; i++)
    {
        if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Monster"),
            LEVEL_GAMEPLAY, strLayerTag)))
            return E_FAIL;
    }*/

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Terrain"),
        LEVEL_GAMEPLAY, strLayerTag)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const _wstring& strLayerTag)
{
    _float fPlayerStateSlotX = g_iWinSizeX * 0.5f;
    _float fPlayerStateSlotY = g_iWinSizeY - 105.f * 0.5f;

    CUI_Image::UIIMAGE_DESC  UIImageDesc
    (TEXT("GameObject_PlayerStateSlot"), CUI_Image::UNCLICKABLE, LEVEL_STATIC, LEVEL_GAMEPLAY,
        fPlayerStateSlotX, fPlayerStateSlotY, 0.9f, 713.f, 105.f,
        L"Prototype_Component_Texture_PlayerStateSlot");

    if (FAILED(m_pGameInstance->Add_UIObject(LEVEL_STATIC, LEVEL_GAMEPLAY,
											 TEXT("Prototype_GameObject_UIImage"),
								   CUI_Manager::PERSISTENT, &UIImageDesc)))
        return E_FAIL;


    CHP_Bar::UI_HPBAR_DESC  UIPlayerHPDesc
    (TEXT("GameObject_PlayerHPBar"), CUIObject::UNCLICKABLE, CHP_Bar::PLAYERHP,
        LEVEL_STATIC, LEVEL_GAMEPLAY,
        fPlayerStateSlotX + 0.3f, fPlayerStateSlotY - 7.f, 0.5f, 86.f, 65.f,
        L"Prototype_Component_Texture_PlayerHP");

    if (FAILED(m_pGameInstance->Add_UIObject(LEVEL_STATIC, LEVEL_GAMEPLAY,
											 TEXT("Prototype_GameObject_UI_HPbar"),
									 CUI_Manager::PERSISTENT, &UIPlayerHPDesc)))
        return E_FAIL;

    return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_GamePlay* pGameInstance = new CLevel_GamePlay(pDevice, pContext);

    if (FAILED(pGameInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_GamePlay");
        Safe_Release(pGameInstance);
    }

    return pGameInstance;
}


void CLevel_GamePlay::Free()
{
    __super::Free();

}
