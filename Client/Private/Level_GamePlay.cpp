#include "Level_GamePlay.h"

#include <iostream>
#include <ostream>
#include <PartObject.h>
#include <UI_Image.h>

#include "GameInstance.h"

#include "Level_Loading.h"
#include "Camera_Free.h"
#include "InventoryBase.h"
#include "InventoryGearSlot.h"
#include "InventoryArtifactSlot.h"
#include "LoungeMap.h"
#include "Player.h"
#include "Zombie.h"
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

    if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
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
        CGameObject* pPlayer = m_pGameInstance->Find_GameObject(TEXT("GameObject_Player"),
            LEVEL_GAMEPLAY, TEXT("Layer_Player"));
        CPlayer* pPlayerHex = dynamic_cast<CPlayer*>(pPlayer);
        pPlayerHex->Show_Player_Inventory(bShowInventory);
    }*/

#ifdef _DEBUG
    if (m_pGameInstance->Key_Down(VK_F1))  // 아예 전체 전역변수로 만들어야겠다
        bMouseClickLock = !bMouseClickLock;
#endif

    _float4     fWorldMousePos = {};
    _float3     fWorldMouseRay = {};
    m_pGameInstance->Compute_MouseRay(fWorldMousePos, fWorldMouseRay);

    // 1. 현재 가장 가까운 Monster collider 찾기
    CCollider* pClosestCollider = Get_Closest_Collider(fWorldMousePos, fWorldMouseRay);
    CMonster* pPrevMonster = m_pPickedMonster;
    CMonster* pCurrMonster = pClosestCollider ?
							dynamic_cast<CMonster*>(pClosestCollider->Get_OwnerObject()) : nullptr;

    // 2. 이전 Hovered 상태 해제
    if (pPrevMonster && pPrevMonster != pCurrMonster)
    {
        pPrevMonster->Set_Hovered(false);

        //std::wcerr << "[휘바 끝XXXXXXXXXXX]" << std::endl;
    }

    // 3. 현재 Hovered 상태 설정 및 클릭 처리
    if (pCurrMonster)
    {
        pCurrMonster->Set_Hovered(true);
        m_pPickedMonster = pCurrMonster;

        //std::wcerr << "[휘바휘바]" << std::endl;

        if (m_pGameInstance->Get_Key(VK_LBUTTON) && !bMouseClickLock)
        {
            Click_Chase_Monster(pCurrMonster);
        }
    }
}

CCollider* CLevel_GamePlay::Get_Closest_Collider(const _float4& mousePos, const _float3& mouseRay)
{
    unordered_map<_wstring, vector<CCollider*>> colliders = *m_pGameInstance->Get_Colliders();
    auto it = colliders.find(TEXT("Monster"));
    if (it == colliders.end()) return nullptr;

    CCollider* pClosest = nullptr;
    _float minDist = FLT_MAX;

    for (auto& pCollider : it->second)
    {
        if (pCollider->Get_ColliderType() != COLLIDER::TYPE_SPHERE)
            continue;

        _float fDist = 0.f;
        CBounding_Sphere::RayDesc rayDesc = {};
        rayDesc.MousePos = { mousePos.x, mousePos.y, mousePos.z };
        rayDesc.MouseRay = mouseRay;
        rayDesc.fDist = &fDist;

        if (pCollider->Get_Bounding()->Intersect(COLLIDER::TYPE_RAY, nullptr, &rayDesc))
        {
            if (fDist < minDist)
            {
                minDist = fDist;
                pClosest = pCollider;
            }
        }
    }

    return pClosest;
}


void CLevel_GamePlay::Click_Chase_Monster(CMonster* pMonster)
{
    if (!pMonster) return;
	
    CTransform* pMonsterTransform = dynamic_cast<CTransform*>(pMonster->Find_Component(TEXT("Com_Transform")));
    if (!pMonsterTransform) return;

    m_pPlayer->Set_Chasing(true, pMonsterTransform);

    //m_pPlayer->Change_State(PLAYER_STATE::WALK);  // 무기 바꾸면 여기 상태도 수정해야 함
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

    Desc.strGameObjectTag = TEXT("GameObject_Camera_Free");
    Desc.vEye = _float3(0.f, 20.f, -15.f);
    Desc.vAt = _float3(0.f, 0.f, 0.f);
    Desc.fFov = XMConvertToRadians(60.f);
    Desc.fNear = 0.01f;
    Desc.fFar = 500.f;
    Desc.fKeySensor = 0.03f;
    Desc.fSpeedPerSec = 8.f;
    Desc.fRotationPerSec = XMConvertToRadians(180.f);

    CGameObject* pCameraObject = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Free"),
        LEVEL_GAMEPLAY, strLayerTag, &Desc);
    if (nullptr == pCameraObject)     return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _wstring& strLayerTag)
{
    CGameObject* pPlayerObject = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_PlayerHex"),
        LEVEL_GAMEPLAY, strLayerTag);
    if (nullptr == pPlayerObject)   return E_FAIL;

    m_pPlayer = dynamic_cast<CPlayer*>(pPlayerObject);

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _wstring& strLayerTag)
{
    CGameObject* pZombie = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Zombie"),
        LEVEL_GAMEPLAY, strLayerTag);
    if (nullptr == pZombie)     return E_FAIL;

    CGameObject* pSkeleton = m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Skeleton"),
        LEVEL_GAMEPLAY, strLayerTag);
    if (nullptr == pSkeleton)     return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
    CGameObject* pLoungeMap = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_LoungeMap"),
        LEVEL_GAMEPLAY, strLayerTag);
    if (nullptr == pLoungeMap)      return E_FAIL;

    /*if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Sky"),
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
