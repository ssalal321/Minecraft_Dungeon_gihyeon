#include "Level_GamePlay.h"

#include <UI_Image.h>

#include "GameInstance.h"

#include "Level_Loading.h"
#include "Camera_Free.h"
#include "InventoryBase.h"
#include "InventoryGearSlot.h"
#include "InventoryItemSlot.h"
#include "LoungeMap.h"
#include "Player.h"
#include "PlayerHP.h"

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

    if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{

    /*if (m_pGameInstance->Key_Down('I'))
    {
        bShowInventory = !bShowInventory;
        CGameObject* pInventoryBase = m_pGameInstance->Find_UIGameObject(TEXT("GameObject_InventoryBase"),
            CUI_Manager::PERSISTENT);
        CUIObject* pUIObject = dynamic_cast<CUIObject*>(pInventoryBase);
        pUIObject->Set_Visible(bShowInventory);
        CGameObject* pPlayer = m_pGameInstance->Find_GameObject(TEXT("Prototype_GameObject_PlayerHex"),
            LEVEL_GAMEPLAY, TEXT("Layer_Player"));
        CPlayer* pPlayerHex = dynamic_cast<CPlayer*>(pPlayer);
        pPlayerHex->Show_Player_Inventory(bShowInventory);
    }*/

    //if (m_pGameInstance->Key_Down(VK_LBUTTON))
    //{
    //    _float3		fWorldPickedPos = {};
    //    _float3		fOutPoints[3] = {} /*nullptr*/;

    //    if (m_pGameInstance->Picked_Model(fWorldPickedPos, TEXT("Prototype_GameObject_LoungeMap"), LEVEL_GAMEPLAY, TEXT("Layer_BackGround"),
    //        fOutPoints))
    //    {
    //        CLoungeMap* pLoungeMap = dynamic_cast<CLoungeMap*>(m_pGameInstance->Find_GameObject(TEXT("Prototype_GameObject_LoungeMap"),
                //											   LEVEL_GAMEPLAY, TEXT("Layer_Player")));

    //        // Navigation에 전달
    //        CNavigation* pNavigation = dynamic_cast<CNavigation*>(pLoungeMap->Find_Component(TEXT("Com_Navigation")));

    //        if (pNavigation != nullptr)
    //        {
    //            pNavigation->Make_Cell(fWorldPickedPos, fOutPoints);
    //        }
    //    }
    //}

    //if (m_pGameInstance->Key_Down(VK_LBUTTON))
    //{
    //    _float3 fWorldPickedPos = {};
    //   
    //    // 2. LoungeMap에 피킹 요청 (BoundingBox 충돌 체크)
    //    if (m_pGameInstance->Picked_Model(fWorldPickedPos, TEXT("Prototype_GameObject_LoungeMap"),
                //						  LEVEL_GAMEPLAY, TEXT("Layer_BackGround")))
    //    {

    //        // 3. 피킹 성공 → 플레이어 이동 요청
    //        CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject(TEXT("Prototype_GameObject_PlayerHex"),
    //            LEVEL_GAMEPLAY, TEXT("Layer_Player")));
    //        pPlayer->Set_NextPosition({ fWorldPickedPos.x, fWorldPickedPos.y, fWorldPickedPos.z, 1.f });
    //        pPlayer->Change_State(PLAYER_STATE::WALK);
    //    }
    //}
   
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

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_PlayerHex"),
        LEVEL_GAMEPLAY, strLayerTag)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _wstring& strLayerTag)
{
    /*for (size_t i = 0; i < 2; i++)
    {
        if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Monster"),
            LEVEL_GAMEPLAY, strLayerTag)))
            return E_FAIL;
    }*/

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
    /*if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Terrain"),
        LEVEL_GAMEPLAY, strLayerTag)))
        return E_FAIL;*/

    if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_LoungeMap"),
        LEVEL_GAMEPLAY, strLayerTag)))
        return E_FAIL;

    /*if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Sky"),
        LEVEL_GAMEPLAY, strLayerTag)))
        return E_FAIL;

    for (size_t i = 0; i < 5; i++)
    {
        if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_ForkLift"),
            LEVEL_GAMEPLAY, strLayerTag)))
            return E_FAIL;
    }*/

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const _wstring& strLayerTag)
{
#pragma region PlayerStateSlot
    _float fPlayerStateSlotX = g_iWinSizeX * 0.5f;
    _float fPlayerStateSlotY = g_iWinSizeY - 105.f * 0.5f;

    CUI_Image::UIIMAGE_DESC  PlayerStateSlotDesc
    (TEXT("GameObject_PlayerStateSlot"), CUI_Image::UNCLICKABLE, 
        fPlayerStateSlotX, fPlayerStateSlotY, 0.9f, 713.f, 105.f,
        L"Prototype_Component_Texture_PlayerStateSlot", LEVEL_STATIC, LEVEL_STATIC);

    CUIObject* pPlayerStateSlot = m_pGameInstance->Add_UIObject(LEVEL_STATIC, LEVEL_STATIC,
        TEXT("Prototype_GameObject_UIImage"),
        CUI_Manager::PERSISTENT, &PlayerStateSlotDesc);

    if (nullptr == pPlayerStateSlot) return E_FAIL;


    CPlayerHP::PLAYERHP_DESC  PlayerHPDesc
    (TEXT("GameObject_PlayerHPBar"), CUIObject::UNCLICKABLE,
        fPlayerStateSlotX + 0.3f, fPlayerStateSlotY - 7.f, 0.7f, 86.f, 65.f,
        L"Prototype_Component_Texture_PlayerHP");

    CUIObject* pPlayerHP = m_pGameInstance->Add_UIObject(LEVEL_STATIC, LEVEL_STATIC,
        TEXT("Prototype_GameObject_Player_HPbar"),
        CUI_Manager::PERSISTENT, &PlayerHPDesc);

    if (nullptr == pPlayerHP) return E_FAIL;
#pragma endregion

#pragma region Inventory
    CInventoryBase::INVENTORY_BASE_DESC  InventoryBaseDesc
    (TEXT("GameObject_InventoryBase"), CUIObject::UNCLICKABLE,
        g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f, 0.6f, 1280.f, 720.f,
        L"Prototype_Component_Texture_InventoryBase");

    CUIObject* pInventoryBase = m_pGameInstance->Add_UIObject(LEVEL_STATIC, LEVEL_GAMEPLAY,
        TEXT("Prototype_GameObject_InventoryBase"),
        CUI_Manager::PERSISTENT, &InventoryBaseDesc);

    if (nullptr == pInventoryBase) return E_FAIL;

    const _float fStartX            = 513.f; // 첫 번째 열의 X 좌표 시작점
    const _float fStartY            = 188.f; // 첫 번째 행의 Y 좌표 시작점
    const _float fRightSlotWidth    = 102.5f; // 슬롯의 너비
    const _float fStoreSlotSpacing  = 10.5f;  // 슬롯 간의 간격

    const _int iColumns             = 3;     // 한 행의 열 수 (3열)
    const _int iRows                = 4;     // 한 열의 행 수 (4행)

    for (_int i = 0; i < iRows * iColumns; ++i)
    {
        _int row = i / iColumns;  // 행 계산
        _int col = i % iColumns;  // 열 계산

        // X, Y 좌표 계산
        _float fXPosition = fStartX + (fRightSlotWidth + fStoreSlotSpacing) * static_cast<float>(col);   // 열에 맞게 X 좌표 계산
        _float fYPosition = fStartY + (fRightSlotWidth + fStoreSlotSpacing) * static_cast<float>(row);  // 행에 맞게 Y 좌표 계산

        std::wstring strSlotName = L"GameObject_InventoryStoreSlot_" + std::to_wstring(i);

        CInventoryBase::INVENTORY_BASE_DESC  InventoryStoreSlotDesc
        (strSlotName.c_str(), CUIObject::CLICKABLE,
            fXPosition, fYPosition, 0.5f, fRightSlotWidth, fRightSlotWidth,
            L"Prototype_Component_Texture_InventoryStoreSlot");

        CUIObject* pInventoryStoreSlot = m_pGameInstance->Add_UIObject(LEVEL_STATIC, LEVEL_GAMEPLAY,
            TEXT("Prototype_GameObject_InventoryStoreSlot"),
            CUI_Manager::PERSISTENT, &InventoryStoreSlotDesc);

        if (nullptr == pInventoryStoreSlot) return E_FAIL;

        pInventoryStoreSlot->Set_Parent(pInventoryBase);  // 부모 설정
    }


    float fLeftSlotsWidth = 80.f; // 슬롯의 너비
    // 근접 무기 슬롯
    CInventoryGearSlot::INVENTORY_GEARSLOT_DESC  InventoryMeleeSlot
    (TEXT("GameObject_InventoryMeleeSlot"), CUIObject::UNCLICKABLE,
        92.f, 202.2f, 0.4f, fLeftSlotsWidth, fLeftSlotsWidth,
        L"Prototype_Component_Texture_InventoryGearSlot");

    CUIObject* pInventoryGearSlot = m_pGameInstance->Add_UIObject(LEVEL_STATIC, LEVEL_GAMEPLAY,
        TEXT("Prototype_GameObject_InventoryGearSlot"),
        CUI_Manager::PERSISTENT, &InventoryMeleeSlot);

    if (nullptr == pInventoryGearSlot) return E_FAIL;
    pInventoryGearSlot->Set_Parent(pInventoryBase);  // 부모 설정

    // 갑옷 슬롯
    CInventoryGearSlot::INVENTORY_GEARSLOT_DESC  InventoryArmorSlotDesc
    (TEXT("GameObject_InventoryArmorSlot"), CUIObject::UNCLICKABLE,
        237.7f, 174.7f, 0.4f, fLeftSlotsWidth, fLeftSlotsWidth,
        L"Prototype_Component_Texture_InventoryGearSlot");

    pInventoryGearSlot = m_pGameInstance->Add_UIObject(LEVEL_STATIC, LEVEL_GAMEPLAY,
        TEXT("Prototype_GameObject_InventoryGearSlot"),
        CUI_Manager::PERSISTENT, &InventoryArmorSlotDesc);

    if (nullptr == pInventoryGearSlot) return E_FAIL;
    pInventoryGearSlot->Set_Parent(pInventoryBase);  // 부모 설정

    // 원거리 무기 슬롯
    CInventoryGearSlot::INVENTORY_GEARSLOT_DESC  InventoryRangedSlotDesc
    (TEXT("GameObject_InventoryRangedSlot"), CUIObject::UNCLICKABLE,
        383.5f, 202.2f, 0.4f, fLeftSlotsWidth, fLeftSlotsWidth,
        L"Prototype_Component_Texture_InventoryGearSlot");

    pInventoryGearSlot = m_pGameInstance->Add_UIObject(LEVEL_STATIC, LEVEL_GAMEPLAY,
        TEXT("Prototype_GameObject_InventoryGearSlot"),
        CUI_Manager::PERSISTENT, &InventoryRangedSlotDesc);

    if (nullptr == pInventoryGearSlot) return E_FAIL;
    pInventoryGearSlot->Set_Parent(pInventoryBase);  // 부모 설정


    // 유물 슬롯
    for (int i = 0; i < 3; ++i)
    {
        float fItemSlotStartX = 126.5f;   // 첫 번째 슬롯의 시작 위치
        
        float fItemSlotSpacing = 31.5f;  // 슬롯 간 간격

        float fSlotX = fItemSlotStartX + static_cast<float>(i) * (fLeftSlotsWidth + fItemSlotSpacing); // 겹치지 않도록 계산

        std::wstring strSlotName = L"GameObject_InventoryItemSlot_Empty_" + std::to_wstring(i);

        CInventoryGearSlot::INVENTORY_GEARSLOT_DESC InventoryItemSlotDesc
        (strSlotName.c_str(), CUIObject::UNCLICKABLE,
            fSlotX, 626.f, 0.4f, 80.f, 80.f,
            L"Prototype_Component_Texture_InventoryItemSlot_Empty");

        CUIObject* pInventoryItemSlot = m_pGameInstance->Add_UIObject(LEVEL_STATIC, LEVEL_GAMEPLAY,
            TEXT("Prototype_GameObject_InventoryItemSlot_Empty"),
            CUI_Manager::PERSISTENT, &InventoryItemSlotDesc);

        if (nullptr == pInventoryItemSlot) return E_FAIL;
        pInventoryItemSlot->Set_Parent(pInventoryBase);  // 부모 설정
    }
#pragma endregion

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
