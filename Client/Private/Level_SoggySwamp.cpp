#include "Level_SoggySwamp.h"

#include <iostream>
#include <UI_Image.h>

#include "BabyZombie.h"
#include "CauldronBoss_Trigger.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Level_Loading.h"
#include "Camera_Target.h"
#include "CauldronBoss.h"
#include "CauldronBossHP.h"
#include "GateFence.h"
#include "InventoryBase.h"
#include "InventoryData.h"
#include "Item.h"
#include "MonsterRush_Trigger.h"
#include "Player.h"
#include "Zombie.h"
#include "PlayerHP.h"
#include "Skeleton.h"
#include "Sky.h"
#include "Slime_Large.h"
#include "Vindicator.h"

CLevel_SoggySwamp::CLevel_SoggySwamp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel { pDevice, pContext }
{
}

HRESULT CLevel_SoggySwamp::Initialize()
{
    if (FAILED(Ready_PrePlayer()))
        return E_FAIL;

    if (FAILED(Ready_Lights()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Boss(TEXT("Layer_Monster"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Trigger(TEXT("Layer_Trigger"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_MonsterRush(TEXT("Layer_Monster"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
        return E_FAIL;
    

    return S_OK;
}

void CLevel_SoggySwamp::Update(_float fTimeDelta)
{
#ifdef _DEBUG
    if (m_pGameInstance->Key_Down(VK_F1))  // 아예 전체 전역변수로 만들어야겠다
        bMouseClickLock = !bMouseClickLock;
#endif
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
        if (pCollider->Get_ColliderType() != COLLIDER_TYPE::TYPE_SPHERE || pCollider->Get_Role() == CCollider::ETC || pCollider->Get_Role() == CCollider::SMALL)
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
}

HRESULT CLevel_SoggySwamp::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
#endif

    return S_OK;
}

HRESULT CLevel_SoggySwamp::Ready_PrePlayer()
{
    CLayer* pPersistentPlayerLayer = m_pGameInstance->Get_Persistent_Layer(TEXT("Layer_Player"));
    if (nullptr == pPersistentPlayerLayer)
        return E_FAIL;

    // Level_SoggySwamp의 m_pLayers에 붙여줌
    if (FAILED(m_pGameInstance->Attach_Persistent_Layer_To_Level(m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Player"))))
        return E_FAIL;

    m_pGameInstance->Attach_Persistent_Colliders_To_Level(m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Player"));

    m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject(TEXT("GameObject_Player"), m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Player")));
    CCollider* pCollider = dynamic_cast<CCollider*>(m_pPlayer->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
    pCollider->Set_IsCollision(false);  // 이전 트리거와 부딪히고 남은 거 지워줌

	m_pPlayer->Erase_Component(TEXT("Com_Navigation"));
    m_pPlayer->Delete_NavigationCom();

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
    // 플레이어가 먼저 준비되어 있어야 함
    if (!m_pPlayer)
        return E_FAIL;

    CTransform* pTargetTransform = dynamic_cast<CTransform*>(m_pPlayer->Find_Component(TEXT("Com_Transform")));
    if (!pTargetTransform)
        return E_FAIL;

    CCamera_Target::CAMERA_TARGET_DESC desc{};
    desc.strGameObjectTag = TEXT("GameObject_Camera_Target");
    desc.pTargetTransform = pTargetTransform;                   // 추적 대상 지정
    desc.vOffset = _float3(-7.f, 13.f, -9.f);              // 뒤쪽 위에서 바라보게
    desc.fLagSpeed = 5.f;
    desc.fFov = XMConvertToRadians(60.f);
    desc.fNear = 0.01f;
    desc.fFar = 500.f;

    CGameObject* pCamera = m_pGameInstance->Add_GameObject(
        LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Target"),
        LEVEL_SOGGYSWAMP, strLayerTag, &desc);

    if (!pCamera)
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_SoggySwamp::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
    CGameObject* pSoggySwampMap = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_SoggySwampMap"),
        LEVEL_SOGGYSWAMP, strLayerTag);
    if (nullptr == pSoggySwampMap)
        return E_FAIL;

    CSky::SKY_DESC desc = {};
    desc.strTexPrototypeTag = TEXT("Prototype_Component_Texture_SoggySwampSky");
    CGameObject* pSky = m_pGameInstance->Add_GameObject(LEVEL_SOGGYSWAMP, TEXT("Prototype_GameObject_Sky"),
									            LEVEL_SOGGYSWAMP, strLayerTag, &desc);
    if (nullptr == pSky)
        return E_FAIL;

    CGateFence::GATEFENCE_DESC  gateFenceDesc0 = {};
    gateFenceDesc0.worldPosition = { 5.5f, 0.f, 51.75f, 1.f };
    gateFenceDesc0.strGameObjectTag = TEXT("GameObject_GateFence_0");
    CGameObject* pGateFence0 = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_GateFence"),
        LEVEL_SOGGYSWAMP, strLayerTag, &gateFenceDesc0);
    if (nullptr == pGateFence0)
        return E_FAIL;

    CGateFence::GATEFENCE_DESC  gateFenceDesc1 = {};
    gateFenceDesc1.worldPosition = { 21.5f, -4.f, 150.75f, 1.f };
    gateFenceDesc1.strGameObjectTag = TEXT("GameObject_GateFence_1");
    CGameObject* pGateFence1 = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_GateFence"),
        LEVEL_SOGGYSWAMP, strLayerTag, &gateFenceDesc1);
    if (nullptr == pGateFence1)
        return E_FAIL;

    CGateFence::GATEFENCE_DESC  gateFenceDesc2 = {};
    gateFenceDesc2.worldPosition = { 21.5f, -4.f, 181.75f, 1.f };
    gateFenceDesc2.strGameObjectTag = TEXT("GameObject_GateFence_2");
    CGameObject* pGateFence2 = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_GateFence"),
        LEVEL_SOGGYSWAMP, strLayerTag, &gateFenceDesc2);
    if (nullptr == pGateFence2)
        return E_FAIL;
  
    /*if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_SOGGYSWAMP, TEXT("Prototype_GameObject_Sky"),
        LEVEL_LOUNGE, strLayerTag)))
        return E_FAIL;
    }*/

    return S_OK;
}

HRESULT CLevel_SoggySwamp::Ready_Layer_Player(const _wstring& strLayerTag)
{
    if (FAILED(m_pPlayer->Ready_Components()))
        return E_FAIL;

    CTransform* pTransformCom = dynamic_cast<CTransform*>(m_pPlayer->Find_Component(TEXT("Com_Transform")));
    pTransformCom->Set_State(CTransform::STATE_POSITION, { 1.f, 3.5f, 5.f , 1.f });

    CNavigation* pNavigationCom = dynamic_cast<CNavigation*>(m_pPlayer->Find_Component(TEXT("Com_Navigation")));
    if (pNavigationCom)
        pNavigationCom->SetUp_CurrentCellIndex(26);

    _float4 currentPosition = {};
    XMStoreFloat4(&currentPosition, pTransformCom->Get_State(CTransform::STATE_POSITION));
    m_pPlayer->Set_NextPosition(currentPosition);

    m_pPlayer->Change_State(PLAYER_STATE::IDLE);

    return S_OK;
}

HRESULT CLevel_SoggySwamp::Ready_Layer_UI(const _wstring& strLayerTag)
{
    _float fCauldronBossHPX = g_iWinSizeX * 0.5f;
    _float fCauldronBossHPY = 78.f;

    CCauldronBossHP::CAULDRONBOSS_HP_DESC  cauldronBossHPDesc
    (TEXT("GameObject_CauldronBoss_HPBar"), CUIObject::UNCLICKABLE,
        fCauldronBossHPX, fCauldronBossHPY, 0.8f, 500.f, 17.f,
        L"Prototype_Component_Texture_CauldronBossHP", m_pCauldronBoss, true);

    CUIObject* pCauldronBossHP = m_pGameInstance->Add_UIObject(LEVEL_STATIC, LEVEL_STATIC,
        TEXT("Prototype_GameObject_CauldronBoss_HPbar"),
        CUI_Manager::TEMPORARY, &cauldronBossHPDesc);

    if (nullptr == pCauldronBossHP)
        return E_FAIL;

    m_pCauldronBoss->Set_My_HPUIObject(pCauldronBossHP);

    return S_OK;
}

HRESULT CLevel_SoggySwamp::Ready_BabyZombie(const _wstring& strLayerTag)
{
    CBabyZombie::BABYZOMBIE_DESC desc{};
    desc.babyZombiePosition = { 20.f, -4.f, 155.25f, 1.f };
    desc.currentCellIndex = 1682;

    CGameObject* pBabyZombie0 = m_pGameInstance->Add_GameObject(
        LEVEL_STATIC, TEXT("Prototype_GameObject_BabyZombie"),
        m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Monster"), &desc);

    if (pBabyZombie0)
    {
        CMonster* pMonster = dynamic_cast<CMonster*>(pBabyZombie0);
        CNavigation* pNavigation = dynamic_cast<CNavigation*>(pMonster->Find_Component(TEXT("Com_Navigation")));
        pNavigation->Lock_Cell(1569);
        pNavigation->Lock_Cell(1688);

        m_pMonsterRush_Trigger->Add_Monster(pMonster);
        pMonster->Set_MyRushTrigger(m_pMonsterRush_Trigger);
        

        CCollider* pColliderBig = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
        pColliderBig->Set_ColliderActive(false);

        CCollider* pColliderSmall = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_SmallSphere")));
        pColliderSmall->Set_ColliderActive(false);

        pMonster->Set_GameObject_Active(false);
    }
    else
        return E_FAIL;

    
    desc.babyZombiePosition = { 29.f, -4.f, 171.25f, 1.f };
    desc.currentCellIndex = 1682;

    CGameObject* pBabyZombie1 = m_pGameInstance->Add_GameObject(
        LEVEL_STATIC, TEXT("Prototype_GameObject_BabyZombie"),
        m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Monster"), &desc);

    if (pBabyZombie1)
    {
        CMonster* pMonster = dynamic_cast<CMonster*>(pBabyZombie1);
        CNavigation* pNavigation = dynamic_cast<CNavigation*>(pMonster->Find_Component(TEXT("Com_Navigation")));
        pNavigation->Lock_Cell(1569);
        pNavigation->Lock_Cell(1688);

        m_pMonsterRush_Trigger->Add_Monster(pMonster);
        pMonster->Set_MyRushTrigger(m_pMonsterRush_Trigger);
        pMonster->Set_GameObject_Active(false);

        CCollider* pColliderBig = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
        pColliderBig->Set_ColliderActive(false);

        CCollider* pColliderSmall = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_SmallSphere")));
        pColliderSmall->Set_ColliderActive(false);
    }
    else
        return E_FAIL;


    desc.babyZombiePosition = { 14.f, -4.f, 177.25f, 1.f };
    desc.currentCellIndex = 1682;

    CGameObject* pBabyZombie2 = m_pGameInstance->Add_GameObject(
        LEVEL_STATIC, TEXT("Prototype_GameObject_BabyZombie"),
        m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Monster"), &desc);

    if (pBabyZombie2)
    {
        CMonster* pMonster = dynamic_cast<CMonster*>(pBabyZombie2);
        CNavigation* pNavigation = dynamic_cast<CNavigation*>(pMonster->Find_Component(TEXT("Com_Navigation")));
        pNavigation->Lock_Cell(1569);
        pNavigation->Lock_Cell(1688);

        m_pMonsterRush_Trigger->Add_Monster(pMonster);
        pMonster->Set_MyRushTrigger(m_pMonsterRush_Trigger);
        pMonster->Set_GameObject_Active(false);

        CCollider* pColliderBig = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
        pColliderBig->Set_ColliderActive(false);

        CCollider* pColliderSmall = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_SmallSphere")));
        pColliderSmall->Set_ColliderActive(false);
    }
    else
        return E_FAIL;


    desc.babyZombiePosition = { 5.f, -4.f, 161.25f, 1.f };
    desc.currentCellIndex = 1682;

    CGameObject* pBabyZombie3 = m_pGameInstance->Add_GameObject(
        LEVEL_STATIC, TEXT("Prototype_GameObject_BabyZombie"),
        m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Monster"), &desc);

    if (pBabyZombie3)
    {
        CMonster* pMonster = dynamic_cast<CMonster*>(pBabyZombie3);
        CNavigation* pNavigation = dynamic_cast<CNavigation*>(pMonster->Find_Component(TEXT("Com_Navigation")));
        pNavigation->Lock_Cell(1569);
        pNavigation->Lock_Cell(1688);

        m_pMonsterRush_Trigger->Add_Monster(pMonster);
        pMonster->Set_MyRushTrigger(m_pMonsterRush_Trigger);
        pMonster->Set_GameObject_Active(false);

        CCollider* pColliderBig = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
        pColliderBig->Set_ColliderActive(false);

        CCollider* pColliderSmall = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_SmallSphere")));
        pColliderSmall->Set_ColliderActive(false);
    }
    else
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_SoggySwamp::Ready_Slime(const _wstring& strLayerTag)
{
    CSlime_Large::SLIME_LARGE_DESC  desc{};
    desc.slimeLargePosition = { 21.f, -4.f, 162.25f, 1.f };
    desc.currentCellIndex = 1682;

    CGameObject* pSlimeLarge0 = m_pGameInstance->Add_GameObject(
        LEVEL_STATIC, TEXT("Prototype_GameObject_Slime_Large"),
        m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Monster"), &desc);

    if (pSlimeLarge0)
    {
        CMonster* pMonster = dynamic_cast<CMonster*>(pSlimeLarge0);
        CNavigation* pNavigation = dynamic_cast<CNavigation*>(pMonster->Find_Component(TEXT("Com_Navigation")));
        pNavigation->Lock_Cell(1569);
        pNavigation->Lock_Cell(1688);

        m_pMonsterRush_Trigger->Add_Monster(pMonster);
        pMonster->Set_MyRushTrigger(m_pMonsterRush_Trigger);
        pMonster->Set_GameObject_Active(false);

        CCollider* pColliderBig = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
        pColliderBig->Set_ColliderActive(false);

        CCollider* pColliderSmall = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_SmallSphere")));
        pColliderSmall->Set_ColliderActive(false);
    }
    else
        return E_FAIL;


    desc.slimeLargePosition = { 21.f, -4.f, 170.25f, 1.f };
    desc.currentCellIndex = 1682;

    CGameObject* pSlimeLarge1 = m_pGameInstance->Add_GameObject(
        LEVEL_STATIC, TEXT("Prototype_GameObject_Slime_Large"),
        m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Monster"), &desc);

    if (pSlimeLarge1)
    {
        CMonster* pMonster = dynamic_cast<CMonster*>(pSlimeLarge1);
        CNavigation* pNavigation = dynamic_cast<CNavigation*>(pMonster->Find_Component(TEXT("Com_Navigation")));
        pNavigation->Lock_Cell(1569);
        pNavigation->Lock_Cell(1688);

        m_pMonsterRush_Trigger->Add_Monster(pMonster);
        pMonster->Set_MyRushTrigger(m_pMonsterRush_Trigger);
        pMonster->Set_GameObject_Active(false);

        CCollider* pColliderBig = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
        pColliderBig->Set_ColliderActive(false);

        CCollider* pColliderSmall = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_SmallSphere")));
        pColliderSmall->Set_ColliderActive(false);
    }
    else
        return E_FAIL;


    desc.slimeLargePosition = { 13.f, -4.f, 170.25f, 1.f };
    desc.currentCellIndex = 1682;

    CGameObject* pSlimeLarge2 = m_pGameInstance->Add_GameObject(
        LEVEL_STATIC, TEXT("Prototype_GameObject_Slime_Large"),
        m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Monster"), &desc);

    if (pSlimeLarge2)
    {
        CMonster* pMonster = dynamic_cast<CMonster*>(pSlimeLarge2);
        CNavigation* pNavigation = dynamic_cast<CNavigation*>(pMonster->Find_Component(TEXT("Com_Navigation")));
        pNavigation->Lock_Cell(1569);
        pNavigation->Lock_Cell(1688);

        m_pMonsterRush_Trigger->Add_Monster(pMonster);
        pMonster->Set_MyRushTrigger(m_pMonsterRush_Trigger);
        pMonster->Set_GameObject_Active(false);

        CCollider* pColliderBig = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
        pColliderBig->Set_ColliderActive(false);

        CCollider* pColliderSmall = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_SmallSphere")));
        pColliderSmall->Set_ColliderActive(false);
    }
    else
        return E_FAIL;


    desc.slimeLargePosition = { 13.f, -4.f, 162.25f, 1.f };
    desc.currentCellIndex = 1682;

    CGameObject* pSlimeLarge3 = m_pGameInstance->Add_GameObject(
        LEVEL_STATIC, TEXT("Prototype_GameObject_Slime_Large"),
        m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Monster"), &desc);

    if (pSlimeLarge3)
    {
        CMonster* pMonster = dynamic_cast<CMonster*>(pSlimeLarge3);
        CNavigation* pNavigation = dynamic_cast<CNavigation*>(pMonster->Find_Component(TEXT("Com_Navigation")));
        pNavigation->Lock_Cell(1569);
        pNavigation->Lock_Cell(1688);

        m_pMonsterRush_Trigger->Add_Monster(pMonster);
        pMonster->Set_MyRushTrigger(m_pMonsterRush_Trigger);
        pMonster->Set_GameObject_Active(false);

        CCollider* pColliderBig = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
        pColliderBig->Set_ColliderActive(false);

        CCollider* pColliderSmall = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_SmallSphere")));
        pColliderSmall->Set_ColliderActive(false);
    }
    else
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_SoggySwamp::Ready_Skeleton(const _wstring& strLayerTag)
{
    CSkeleton::SKELETON_DESC  desc{};
    desc.skeletonPosition = { 25.f, -4.f, 166.25f, 1.f };
    desc.currentCellIndex = 1682;

    CGameObject* pSkeleton0 = m_pGameInstance->Add_GameObject(
        LEVEL_STATIC, TEXT("Prototype_GameObject_Skeleton"),
        m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Monster"), &desc);

    if (pSkeleton0)
    {
        CMonster* pMonster = dynamic_cast<CMonster*>(pSkeleton0);
        CNavigation* pNavigation = dynamic_cast<CNavigation*>(pMonster->Find_Component(TEXT("Com_Navigation")));
        pNavigation->Lock_Cell(1569);
        pNavigation->Lock_Cell(1688);

        m_pMonsterRush_Trigger->Add_Monster(pMonster);
        pMonster->Set_MyRushTrigger(m_pMonsterRush_Trigger);
        pMonster->Set_GameObject_Active(false);

        CCollider* pColliderBig = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
        pColliderBig->Set_ColliderActive(false);

        CCollider* pColliderSmall = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_SmallSphere")));
        pColliderSmall->Set_ColliderActive(false);
    }
    else
        return E_FAIL;


    desc.skeletonPosition = { 17.f, -4.f, 174.25f, 1.f };
    desc.currentCellIndex = 1682;

    CGameObject* pSkeleton1 = m_pGameInstance->Add_GameObject(
        LEVEL_STATIC, TEXT("Prototype_GameObject_Skeleton"),
        m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Monster"), &desc);

    if (pSkeleton1)
    {
        CMonster* pMonster = dynamic_cast<CMonster*>(pSkeleton1);
        CNavigation* pNavigation = dynamic_cast<CNavigation*>(pMonster->Find_Component(TEXT("Com_Navigation")));
        pNavigation->Lock_Cell(1569);
        pNavigation->Lock_Cell(1688);

        m_pMonsterRush_Trigger->Add_Monster(pMonster);
        pMonster->Set_MyRushTrigger(m_pMonsterRush_Trigger);
        pMonster->Set_GameObject_Active(false);

        CCollider* pColliderBig = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
        pColliderBig->Set_ColliderActive(false);

        CCollider* pColliderSmall = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_SmallSphere")));
        pColliderSmall->Set_ColliderActive(false);
    }
    else
        return E_FAIL;


    desc.skeletonPosition = { 9.f, -4.f, 166.25f, 1.f };
    desc.currentCellIndex = 1682;

    CGameObject* pSkeleton2 = m_pGameInstance->Add_GameObject(
        LEVEL_STATIC, TEXT("Prototype_GameObject_Skeleton"),
        m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Monster"), &desc);

    if (pSkeleton2)
    {
        CMonster* pMonster = dynamic_cast<CMonster*>(pSkeleton2);
        CNavigation* pNavigation = dynamic_cast<CNavigation*>(pMonster->Find_Component(TEXT("Com_Navigation")));
        pNavigation->Lock_Cell(1569);
        pNavigation->Lock_Cell(1688);

        m_pMonsterRush_Trigger->Add_Monster(pMonster);
        pMonster->Set_MyRushTrigger(m_pMonsterRush_Trigger);
        pMonster->Set_GameObject_Active(false);

        CCollider* pColliderBig = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
        pColliderBig->Set_ColliderActive(false);

        CCollider* pColliderSmall = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_SmallSphere")));
        pColliderSmall->Set_ColliderActive(false);
    }
    else
        return E_FAIL;


    desc.skeletonPosition = { 17.f, -4.f, 158.25f, 1.f };
    desc.currentCellIndex = 1682;

    CGameObject* pSkeleton3 = m_pGameInstance->Add_GameObject(
        LEVEL_STATIC, TEXT("Prototype_GameObject_Skeleton"),
        m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Monster"), &desc);

    if (pSkeleton3)
    {
        CMonster* pMonster = dynamic_cast<CMonster*>(pSkeleton3);
        CNavigation* pNavigation = dynamic_cast<CNavigation*>(pMonster->Find_Component(TEXT("Com_Navigation")));
        pNavigation->Lock_Cell(1569);
        pNavigation->Lock_Cell(1688);

        m_pMonsterRush_Trigger->Add_Monster(pMonster);
        pMonster->Set_MyRushTrigger(m_pMonsterRush_Trigger);
        pMonster->Set_GameObject_Active(false);

        CCollider* pColliderBig = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
        pColliderBig->Set_ColliderActive(false);

        CCollider* pColliderSmall = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_SmallSphere")));
        pColliderSmall->Set_ColliderActive(false);
    }
    else
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_SoggySwamp::Ready_Vindicator(const _wstring& strLayerTag)
{
    CVindicator::VINDICATOR_DESC  desc{};
    desc.vindicatorPosition = { 29.f, -4.f, 161.25f, 1.f };
    desc.currentCellIndex = 1682;

    CGameObject* pVindicator0 = m_pGameInstance->Add_GameObject(
        LEVEL_STATIC, TEXT("Prototype_GameObject_Vindicator"),
        m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Monster"), &desc);

    if (pVindicator0)
    {
        CMonster* pMonster = dynamic_cast<CMonster*>(pVindicator0);
        CNavigation* pNavigation = dynamic_cast<CNavigation*>(pMonster->Find_Component(TEXT("Com_Navigation")));
        pNavigation->Lock_Cell(1569);
        pNavigation->Lock_Cell(1688);

        m_pMonsterRush_Trigger->Add_Monster(pMonster);
        pMonster->Set_MyRushTrigger(m_pMonsterRush_Trigger);
        pMonster->Set_GameObject_Active(false);

        CCollider* pColliderBig = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
        pColliderBig->Set_ColliderActive(false);

        CCollider* pColliderSmall = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_SmallSphere")));
        pColliderSmall->Set_ColliderActive(false);
    }
    else
        return E_FAIL;


    desc.vindicatorPosition = { 20.f, -4.f, 177.25f, 1.f };
    desc.currentCellIndex = 1682;

    CGameObject* pVindicator1 = m_pGameInstance->Add_GameObject(
        LEVEL_STATIC, TEXT("Prototype_GameObject_Vindicator"),
        m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Monster"), &desc);

    if (pVindicator1)
    {
        CMonster* pMonster = dynamic_cast<CMonster*>(pVindicator1);
        CNavigation* pNavigation = dynamic_cast<CNavigation*>(pMonster->Find_Component(TEXT("Com_Navigation")));
        pNavigation->Lock_Cell(1569);
        pNavigation->Lock_Cell(1688);

        m_pMonsterRush_Trigger->Add_Monster(pMonster);
        pMonster->Set_MyRushTrigger(m_pMonsterRush_Trigger);
        pMonster->Set_GameObject_Active(false);

        CCollider* pColliderBig = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
        pColliderBig->Set_ColliderActive(false);

        CCollider* pColliderSmall = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_SmallSphere")));
        pColliderSmall->Set_ColliderActive(false);
    }
    else
        return E_FAIL;


    desc.vindicatorPosition = { 5.f, -4.f, 171.25f, 1.f };
    desc.currentCellIndex = 1682;

    CGameObject* pVindicator2 = m_pGameInstance->Add_GameObject(
        LEVEL_STATIC, TEXT("Prototype_GameObject_Vindicator"),
        m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Monster"), &desc);

    if (pVindicator2)
    {
        CMonster* pMonster = dynamic_cast<CMonster*>(pVindicator2);
        CNavigation* pNavigation = dynamic_cast<CNavigation*>(pMonster->Find_Component(TEXT("Com_Navigation")));
        pNavigation->Lock_Cell(1569);
        pNavigation->Lock_Cell(1688);

        m_pMonsterRush_Trigger->Add_Monster(pMonster);
        pMonster->Set_MyRushTrigger(m_pMonsterRush_Trigger);
        pMonster->Set_GameObject_Active(false);

        CCollider* pColliderBig = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
        pColliderBig->Set_ColliderActive(false);

        CCollider* pColliderSmall = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_SmallSphere")));
        pColliderSmall->Set_ColliderActive(false);
    }
    else
        return E_FAIL;


    desc.vindicatorPosition = { 14.f, -4.f, 155.25f, 1.f };
    desc.currentCellIndex = 1682;

    CGameObject* pVindicator3 = m_pGameInstance->Add_GameObject(
        LEVEL_STATIC, TEXT("Prototype_GameObject_Vindicator"),
        m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Monster"), &desc);

    if (pVindicator3)
    {
        CMonster* pMonster = dynamic_cast<CMonster*>(pVindicator3);
        CNavigation* pNavigation = dynamic_cast<CNavigation*>(pMonster->Find_Component(TEXT("Com_Navigation")));
        pNavigation->Lock_Cell(1569);
        pNavigation->Lock_Cell(1688);

        m_pMonsterRush_Trigger->Add_Monster(pMonster);
        pMonster->Set_MyRushTrigger(m_pMonsterRush_Trigger);
        pMonster->Set_GameObject_Active(false);

        CCollider* pColliderBig = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
        pColliderBig->Set_ColliderActive(false);

        CCollider* pColliderSmall = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_SmallSphere")));
        pColliderSmall->Set_ColliderActive(false);
    }
    else
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_SoggySwamp::Ready_Zombie(const _wstring& strLayerTag)
{
    CZombie::ZOMBIE_DESC  desc{};
    desc.zombiePosition     = { 25.f, -4.f, 163.25f, 1.f };
    desc.currentCellIndex   = 1682;

    CGameObject* pZombie0 = m_pGameInstance->Add_GameObject(
        LEVEL_STATIC, TEXT("Prototype_GameObject_Zombie"),
        m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Monster"), &desc);

    if (pZombie0)
    {
        CMonster* pMonster = dynamic_cast<CMonster*>(pZombie0);
        CNavigation* pNavigation = dynamic_cast<CNavigation*>(pMonster->Find_Component(TEXT("Com_Navigation")));
        pNavigation->Lock_Cell(1569);
        pNavigation->Lock_Cell(1688);

        m_pMonsterRush_Trigger->Add_Monster(pMonster);
        pMonster->Set_MyRushTrigger(m_pMonsterRush_Trigger);
        pMonster->Set_GameObject_Active(false);

        CCollider* pColliderBig = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
        pColliderBig->Set_ColliderActive(false);

        CCollider* pColliderSmall = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_SmallSphere")));
        pColliderSmall->Set_ColliderActive(false);
    }
    else
        return E_FAIL;


    desc.zombiePosition = { 25.f, -4.f, 169.25f, 1.f };
    desc.currentCellIndex = 1682;

    CGameObject* pZombie1 = m_pGameInstance->Add_GameObject(
        LEVEL_STATIC, TEXT("Prototype_GameObject_Zombie"),
        m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Monster"), &desc);

    if (pZombie1)
    {
        CMonster* pMonster = dynamic_cast<CMonster*>(pZombie1);
        CNavigation* pNavigation = dynamic_cast<CNavigation*>(pMonster->Find_Component(TEXT("Com_Navigation")));
        pNavigation->Lock_Cell(1569);
        pNavigation->Lock_Cell(1688);

        m_pMonsterRush_Trigger->Add_Monster(pMonster);
        pMonster->Set_MyRushTrigger(m_pMonsterRush_Trigger);
        pMonster->Set_GameObject_Active(false);

        CCollider* pColliderBig = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
        pColliderBig->Set_ColliderActive(false);

        CCollider* pColliderSmall = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_SmallSphere")));
        pColliderSmall->Set_ColliderActive(false);
    }
    else
        return E_FAIL;


    desc.zombiePosition = { 9.f, -4.f, 169.25f, 1.f };
    desc.currentCellIndex = 1682;

    CGameObject* pZombie2 = m_pGameInstance->Add_GameObject(
        LEVEL_STATIC, TEXT("Prototype_GameObject_Zombie"),
        m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Monster"), &desc);

    if (pZombie2)
    {
        CMonster* pMonster = dynamic_cast<CMonster*>(pZombie2);
        CNavigation* pNavigation = dynamic_cast<CNavigation*>(pMonster->Find_Component(TEXT("Com_Navigation")));
        pNavigation->Lock_Cell(1569);
        pNavigation->Lock_Cell(1688);

        m_pMonsterRush_Trigger->Add_Monster(pMonster);
        pMonster->Set_MyRushTrigger(m_pMonsterRush_Trigger);
        pMonster->Set_GameObject_Active(false);

        CCollider* pColliderBig = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
        pColliderBig->Set_ColliderActive(false);

        CCollider* pColliderSmall = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_SmallSphere")));
        pColliderSmall->Set_ColliderActive(false);
    }
    else
        return E_FAIL;


    desc.zombiePosition = { 9.f, -4.f, 163.25f, 1.f };
    desc.currentCellIndex = 1682;

    CGameObject* pZombie3 = m_pGameInstance->Add_GameObject(
        LEVEL_STATIC, TEXT("Prototype_GameObject_Zombie"),
        m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Monster"), &desc);

    if (pZombie3)
    {
        CMonster* pMonster = dynamic_cast<CMonster*>(pZombie3);
        CNavigation* pNavigation = dynamic_cast<CNavigation*>(pMonster->Find_Component(TEXT("Com_Navigation")));
        pNavigation->Lock_Cell(1569);
        pNavigation->Lock_Cell(1688);

        m_pMonsterRush_Trigger->Add_Monster(pMonster);
        pMonster->Set_MyRushTrigger(m_pMonsterRush_Trigger);
        pMonster->Set_GameObject_Active(false);

        CCollider* pColliderBig = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
        pColliderBig->Set_ColliderActive(false);

        CCollider* pColliderSmall = dynamic_cast<CCollider*>(pMonster->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_SmallSphere")));
        pColliderSmall->Set_ColliderActive(false);
    }
    else
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_SoggySwamp::Ready_Layer_Boss(const _wstring& strLayerTag)
{
    CCauldronBoss::CAULDRONBOSS_DESC  cauldronBossDesc = {};
    cauldronBossDesc.slimeCauldronPosition = { 0.45f, 0, 29.8f, 1.f };

    CGameObject* pCauldronBoss = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_CauldronBoss"),
																 LEVEL_SOGGYSWAMP, strLayerTag, &cauldronBossDesc);
    if (nullptr == pCauldronBoss)
        return E_FAIL;

    m_pCauldronBoss = dynamic_cast<CCauldronBoss*>(pCauldronBoss);

    CCollider* pColliderBig = dynamic_cast<CCollider*>(m_pCauldronBoss->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
    pColliderBig->Set_ColliderActive(false);

    CCollider* pColliderSmall = dynamic_cast<CCollider*>(m_pCauldronBoss->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_SmallSphere")));
    pColliderSmall->Set_ColliderActive(false);

    return S_OK;
}

HRESULT CLevel_SoggySwamp::Ready_Layer_MonsterRush(const _wstring& strLayerTag)
{
    if (FAILED(Ready_BabyZombie(strLayerTag)))
        return E_FAIL;

    if (FAILED(Ready_Slime(strLayerTag)))
        return E_FAIL;

    if (FAILED(Ready_Skeleton(strLayerTag)))
        return E_FAIL;

    if (FAILED(Ready_Vindicator(strLayerTag)))
        return E_FAIL;

    if (FAILED(Ready_Zombie(strLayerTag)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_SoggySwamp::Ready_Layer_Trigger(const _wstring& strLayerTag)
{
    CCauldronBoss_Trigger::BOSS_TRIGGER_DESC   bossTriggerDesc = {};
    bossTriggerDesc.triggerPosition = { -4.f, 0.f, 22.5f };
    bossTriggerDesc.pBoss = m_pCauldronBoss;

    m_pBoss_Trigger = CCauldronBoss_Trigger::Create(m_pDevice, m_pContext, &bossTriggerDesc);
    if (nullptr == m_pBoss_Trigger)
        return E_FAIL;

    CMonsterRush_Trigger::MONSTERRUSH_TRIGGER_DESC   monsterTriggerDesc = {};
    monsterTriggerDesc.triggerPosition = { 17.f, -2.f, 166.25f };

    m_pMonsterRush_Trigger = CMonsterRush_Trigger::Create(m_pDevice, m_pContext, &monsterTriggerDesc);
    if (nullptr == m_pMonsterRush_Trigger)
        return E_FAIL;

    m_pGameInstance->Add_To_Layer(m_pMonsterRush_Trigger, LEVEL_SOGGYSWAMP, strLayerTag, TEXT("GameObject_MonsterRush_Trigger"));

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

    Safe_Release(m_pBoss_Trigger);
}
