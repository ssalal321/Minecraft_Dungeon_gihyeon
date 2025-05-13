#include "Level_SoggySwamp.h"

#include <iostream>
#include <UI_Image.h>

#include "GameInstance.h"
#include "PartObject.h"
#include "Level_Loading.h"
#include "Camera_Free.h"
#include "InventoryBase.h"
#include "InventoryData.h"
#include "Item.h"
#include "Player.h"
#include "Zombie.h"
#include "PlayerHP.h"

CLevel_SoggySwamp::CLevel_SoggySwamp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel { pDevice, pContext }
{
}

HRESULT CLevel_SoggySwamp::Initialize()
{
    if (FAILED(Ready_Lights()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    /*if (FAILED(Ready_Layer_InventoryUI(TEXT("Layer_InventoryUI"))))
        return E_FAIL;*/

    if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
        return E_FAIL;

    /*if (FAILED(Ready_Layer_PlayerSlotUI(TEXT("Layer_PlayerSlotUI"))))
        return E_FAIL;*/

    if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
        return E_FAIL;


    return S_OK;
}

void CLevel_SoggySwamp::Update(_float fTimeDelta)
{
#ifdef _DEBUG
    if (m_pGameInstance->Key_Down(VK_F1))  // 아예 전체 전역변수로 만들어야겠다
        bMouseClickLock = !bMouseClickLock;
#endif

    // 얘네도 여러 level에서 써야 하니까 state_monster로 빼는 게 나을지도..
    _float4     fWorldMousePos = {};
    _float3     fWorldMouseRay = {};
    m_pGameInstance->Compute_MouseRay(fWorldMousePos, fWorldMouseRay);

    // 1. 현재 가장 가까운 Monster collider 찾기
    CCollider* pClosestCollider = Get_Closest_Collider(fWorldMousePos, fWorldMouseRay);
    if (nullptr == pClosestCollider)  // 아래에 다른 코드 없기도 하고 나중에 함수로 뺄 생각 하고 넣은 것
        return;

    CMonster* pPrevMonster = m_pPickedMonster;
    CMonster* pCurrMonster = dynamic_cast<CMonster*>(dynamic_cast<CPartObject*>(pClosestCollider->Get_OwnerObject())->Get_ContainerObject());

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

        std::wcerr << "[휘바휘바]" << std::endl;

        if (m_pGameInstance->Get_Key(VK_LBUTTON) && !bMouseClickLock)
        {
            Click_Chase_Monster(pCurrMonster);
        }
    }

    if (m_pGameInstance->Key_Up(VK_LBUTTON) && !bMouseClickLock)
    {
        m_pPlayer->Set_Chasing(false);
    }
}

CCollider* CLevel_SoggySwamp::Get_Closest_Collider(const _float4& mousePos, const _float3& mouseRay)
{
    unordered_map<_wstring, vector<CCollider*>> colliders = *m_pGameInstance->Get_Colliders(m_pGameInstance->Get_CurrentLevelIndex());
    auto it = colliders.find(TEXT("Monster"));
    if (it == colliders.end())
        return nullptr;

    CCollider* pClosest = nullptr;
    _float minDist = FLT_MAX;

    for (auto& pCollider : it->second)
    {
        if (pCollider->Get_ColliderType() != COLLIDER_TYPE::TYPE_SPHERE)
            continue;

        _float fDist = 0.f;
        CBounding_Sphere::RayDesc rayDesc = {};
        rayDesc.MousePos = { mousePos.x, mousePos.y, mousePos.z };
        rayDesc.MouseRay = mouseRay;
        rayDesc.fDist = &fDist;

        if (pCollider->Get_Bounding()->Intersect(COLLIDER_TYPE::TYPE_RAY, nullptr, &rayDesc))
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


void CLevel_SoggySwamp::Click_Chase_Monster(CMonster* pMonster)
{
    if (!pMonster) return;

    _float4 monsterPickedPos = { 0.f, 0.f, 0.f, 1.f };

    CTransform* pMonsterTransform = dynamic_cast<CTransform*>(pMonster->Find_Component(TEXT("Com_Transform")));
    if (!pMonsterTransform) return;

    m_pPlayer->Set_Chasing(true, pMonsterTransform);

    XMStoreFloat4(&monsterPickedPos, pMonsterTransform->Get_State(CTransform::STATE_POSITION));
    m_pPlayer->Set_MonsterPickedPos(monsterPickedPos);

    //m_pPlayer->Change_State(PLAYER_STATE::WALK);  // 무기 바꾸면 여기 상태도 수정해야 함
}

HRESULT CLevel_SoggySwamp::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
#endif

    return S_OK;
}

HRESULT CLevel_SoggySwamp::Ready_Lights()
{
    LIGHT_DESC          LightDesc{};

    LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

    if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
        return E_FAIL;

    LightDesc.eType = LIGHT_DESC::TYPE_POINT;
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
    LightDesc.vPosition = _float4(20.f, 5.f, 20.f, 1.f);
    LightDesc.fRange = 20.f;
    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

    if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_SoggySwamp::Ready_Layer_Camera(const _wstring& strLayerTag)
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
        LEVEL_SOGGYSWAMP, strLayerTag, &Desc);
    if (nullptr == pCameraObject)     return E_FAIL;

    return S_OK;
}

HRESULT CLevel_SoggySwamp::Ready_Layer_Player(const _wstring& strLayerTag)
{
    // 이전 Level의 Layer_Player 가져옴
    CLayer* pPersistentPlayerLayer = m_pGameInstance->Get_Persistent_Layer(TEXT("Layer_Player"));
    if (nullptr == pPersistentPlayerLayer)
        return E_FAIL;

    // Level_SoggySwamp의 m_pLayers에 붙여줌
    if (FAILED(m_pGameInstance->Attach_Persistent_Layer_To_Level(m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Player"))))
        return E_FAIL;

    m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject(TEXT("GameObject_Player"), m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Player")));

    // 이전 Level에서의 Collider도 보존
    m_pGameInstance->Attach_Persistent_Colliders_To_Level(m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Player"));

    m_pPlayer->Erase_Component(TEXT("Com_Navigation_LoungeMap"));

    CComponent* pNavigationCom = m_pPlayer->Find_Component(TEXT("Com_Navigation_LoungeMap"));
    // ☆☆☆☆☆ navigationCom 재정비해줘야 할 듯 ☆☆☆☆☆

    m_pPlayer->Delete_NavigationCom();

    CTransform* pTransformCom = dynamic_cast<CTransform*>(m_pPlayer->Find_Component(TEXT("Com_Transform")));
    pTransformCom->Set_State(CTransform::STATE_POSITION, { 0.f, 0.f, 0.f , 1.f });

    return S_OK;
}

HRESULT CLevel_SoggySwamp::Ready_Layer_Monster(const _wstring& strLayerTag)
{
    CGameObject* pZombie = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Zombie"),
        LEVEL_SOGGYSWAMP, strLayerTag);
    if (nullptr == pZombie)     return E_FAIL;

    CGameObject* pSkeleton = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Skeleton"),
        LEVEL_SOGGYSWAMP, strLayerTag);
    if (nullptr == pSkeleton)     return E_FAIL;

    return S_OK;
}

HRESULT CLevel_SoggySwamp::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
    CGameObject* pSoggySwampMap = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_SoggySwampMap"),
        LEVEL_SOGGYSWAMP, strLayerTag);
    if (nullptr == pSoggySwampMap)      return E_FAIL;

    /*if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_SOGGYSWAMP, TEXT("Prototype_GameObject_Sky"),
        LEVEL_LOUNGE, strLayerTag)))
        return E_FAIL;
    }*/

    return S_OK;
}

HRESULT CLevel_SoggySwamp::Ready_Layer_InventoryUI(const _wstring& strLayerTag)
{
    CInventoryBase::INVENTORY_BASE_DESC  InventoryBaseDesc
    (TEXT("GameObject_InventoryBase"), CUIObject::UNCLICKABLE,
        g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f, 0.6f, 1280.f, 720.f,
        L"Prototype_Component_Texture_InventoryBase");

    CUIObject* pInventoryBase = m_pGameInstance->Add_UIObject(LEVEL_STATIC, LEVEL_SOGGYSWAMP,
        TEXT("Prototype_GameObject_InventoryBase"),
        CUI_Manager::PERSISTENT, &InventoryBaseDesc);

    if (nullptr == pInventoryBase) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_SoggySwamp::Ready_Layer_PlayerSlotUI(const _wstring& strLayerTag)
{
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

    return S_OK;
}

CLevel_SoggySwamp* CLevel_SoggySwamp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_SoggySwamp* pGameInstance = new CLevel_SoggySwamp(pDevice, pContext);

    if (FAILED(pGameInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_SoggySwamp");
        Safe_Release(pGameInstance);
    }

    return pGameInstance;
}


void CLevel_SoggySwamp::Free()
{
    __super::Free();
}
