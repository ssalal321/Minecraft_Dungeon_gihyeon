#include "Level_Lounge.h"

#include <UI_Image.h>

#include "GameInstance.h"
#include "PartObject.h"
#include "Level_Loading.h"
#include "Camera_Free.h"
#include "InventoryBase.h"
#include "InventoryData.h"
#include "Item.h"
#include "Level_Trigger.h"
#include "LoungeMap.h"
#include "Player.h"
#include "Zombie.h"
#include "PlayerHP.h"

CLevel_Lounge::CLevel_Lounge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Lounge::Initialize()
{
    if (FAILED(Ready_Lights()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_InventoryUI(TEXT("Layer_InventoryUI"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_PlayerSlotUI(TEXT("Layer_PlayerSlotUI"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
        return E_FAIL;

    CLevel_Trigger::LEVEL_TRIGGER_DESC pDesc = {};
    pDesc.triggerPosition = { 2.5f, 5.5f, 15.f };
    m_pLevel_Trigger = CLevel_Trigger::Create(m_pDevice, m_pContext, &pDesc);
    if (nullptr == m_pLevel_Trigger)
        return E_FAIL;

#pragma region MELEE
    CItem::ITEM_DESC	ItemDesc{};

    CModel* pBody = dynamic_cast<CModel*>(m_pPlayer->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Model")));
    if (nullptr == pBody)
        return E_FAIL;

    _float4x4* pPlayerWorldMatrixPtr = dynamic_cast<CTransform*>(m_pPlayer->Find_Component(TEXT("Com_Transform")))->Get_WorldMatrix_Ptr();

    ItemDesc.pParentWorldMatrix = pPlayerWorldMatrixPtr;
    ItemDesc.pState = &m_pPlayer->Get_PlayerState();
    ItemDesc.pSocketMatrix = pBody->Get_CombinedTransformationMatrix("J_R_Weapon");
    ItemDesc.pContainerObject = m_pPlayer;
    ItemDesc.pCollisionActivating = &m_pPlayer->Get_Attacking();

    m_pPlayer->Get_InventoryData()->Add_Item_To_StoreSlot(LEVEL_STATIC, TEXT("Prototype_GameObject_Glaive_Steel"), TEXT("Part_Weapon_Glaive"), &ItemDesc);
#pragma endregion

#pragma region RANGED
    CItem::ITEM_DESC	BowDesc{};

    BowDesc.pParentWorldMatrix = pPlayerWorldMatrixPtr;
    BowDesc.pState = &m_pPlayer->Get_PlayerState();
    BowDesc.pSocketMatrix = pBody->Get_CombinedTransformationMatrix("J_L_Weapon");
    BowDesc.pContainerObject = m_pPlayer;
    BowDesc.pCollisionActivating = &m_pPlayer->Get_Attacking();

    m_pPlayer->Get_InventoryData()->Add_Item_To_StoreSlot(LEVEL_STATIC, TEXT("Prototype_GameObject_Bow"), TEXT("Part_Weapon_Bow"), &BowDesc);

#pragma endregion 

    return S_OK;
}

void CLevel_Lounge::Update(_float fTimeDelta)
{
    if (m_pLevel_Trigger->Get_Level_Change())
    {
        if (SUCCEEDED(m_pGameInstance->Open_Level(LEVEL_LOADING,
            CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_SOGGYSWAMP))))
            return;
    }


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

        //std::wcerr << "[휘바휘바]" << std::endl;

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

CCollider* CLevel_Lounge::Get_Closest_Collider(const _float4& mousePos, const _float3& mouseRay)
{
    unordered_map<_wstring, vector<CCollider*>> colliders = *m_pGameInstance->Get_Colliders(m_pGameInstance->Get_CurrentLevelIndex());
    auto it = colliders.find(TEXT("Monster"));
    if (it == colliders.end())
        return nullptr;

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


void CLevel_Lounge::Click_Chase_Monster(CMonster* pMonster)
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

HRESULT CLevel_Lounge::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
#endif

    return S_OK;
}

HRESULT CLevel_Lounge::Ready_Lights()
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

HRESULT CLevel_Lounge::Ready_Layer_Camera(const _wstring& strLayerTag)
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
        LEVEL_LOUNGE, strLayerTag, &Desc);
    if (nullptr == pCameraObject)     return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Lounge::Ready_Layer_Player(const _wstring& strLayerTag)
{
    const _uint currentLevel = m_pGameInstance->Get_ChangedLevelIndex();

    // 1. 현재 레벨에 이미 존재하면 바로 가져오기
    CLayer* pExistingLayer = m_pGameInstance->Find_Layer(currentLevel, strLayerTag);
    if (pExistingLayer != nullptr)
    {
        m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject(TEXT("GameObject_Player"), currentLevel, strLayerTag));
        return S_OK;
    }

    // 2. Persistent에만 존재하는 경우 -> 현재 레벨에 붙이기
    CLayer* pPersistentLayer = m_pGameInstance->Get_Persistent_Layer(strLayerTag);
    if (pPersistentLayer != nullptr)
    {
        if (FAILED(m_pGameInstance->Attach_Persistent_Layer_To_Level(currentLevel, strLayerTag)))
            return E_FAIL;

        m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject(TEXT("GameObject_Player"), currentLevel, strLayerTag));
        return S_OK;
    }

    // 3. 어디에도 없으면 생성
    CGameObject* pPlayerObject = m_pGameInstance->Add_GameObject(LEVEL_STATIC,
													TEXT("Prototype_GameObject_PlayerHex"),
												LEVEL_LOUNGE, strLayerTag);
    if (nullptr == pPlayerObject)
        return E_FAIL;

    m_pPlayer = dynamic_cast<CPlayer*>(pPlayerObject);

    // 생성 후 persistent 등록
    if (FAILED(m_pGameInstance->Set_Layer_Persistent(LEVEL_LOUNGE, strLayerTag)))
        return E_FAIL;

    return S_OK;
}


HRESULT CLevel_Lounge::Ready_Layer_Monster(const _wstring& strLayerTag)
{
    CGameObject* pZombie = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Zombie"),
        LEVEL_LOUNGE, strLayerTag);
    if (nullptr == pZombie)     return E_FAIL;

    CGameObject* pSkeleton = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Skeleton"),
        LEVEL_LOUNGE, strLayerTag);
    if (nullptr == pSkeleton)     return E_FAIL;

    CGameObject* pBabyZombie = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_BabyZombie"),
        LEVEL_LOUNGE, strLayerTag);
    if (nullptr == pBabyZombie)     return E_FAIL;

    CGameObject* pSlimeLarge = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Slime_Large"),
        LEVEL_LOUNGE, strLayerTag);
    if (nullptr == pSlimeLarge)     return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Lounge::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
    CGameObject* pLoungeMap = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_LoungeMap"),
        LEVEL_LOUNGE, strLayerTag);
    if (nullptr == pLoungeMap)      return E_FAIL;

    /*if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_LOUNGE, TEXT("Prototype_GameObject_Sky"),
        LEVEL_LOUNGE, strLayerTag)))
        return E_FAIL;
    }*/

    return S_OK;
}

HRESULT CLevel_Lounge::Ready_Layer_InventoryUI(const _wstring& strLayerTag)
{
    CInventoryBase::INVENTORY_BASE_DESC  InventoryBaseDesc
    (TEXT("GameObject_InventoryBase"), CUIObject::UNCLICKABLE,
        g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f, 0.6f, 1280.f, 720.f,
        L"Prototype_Component_Texture_InventoryBase");

    CUIObject* pInventoryBase = m_pGameInstance->Add_UIObject(LEVEL_STATIC, LEVEL_STATIC,
        TEXT("Prototype_GameObject_InventoryBase"),
        CUI_Manager::PERSISTENT, &InventoryBaseDesc);

    if (nullptr == pInventoryBase) return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Lounge::Ready_Layer_PlayerSlotUI(const _wstring& strLayerTag)
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

CLevel_Lounge* CLevel_Lounge::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Lounge* pGameInstance = new CLevel_Lounge(pDevice, pContext);

    if (FAILED(pGameInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_Lounge");
        Safe_Release(pGameInstance);
    }

    return pGameInstance;
}


void CLevel_Lounge::Free()
{
    __super::Free();

    Safe_Release(m_pLevel_Trigger);
}
