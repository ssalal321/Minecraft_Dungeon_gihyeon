#include "Level_SoggySwamp.h"

#include <iostream>
#include <UI_Image.h>

#include "Boss_Trigger.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Level_Loading.h"
#include "Camera_Free.h"
#include "CauldronBoss.h"
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

    CBoss_Trigger::BOSS_TRIGGER_DESC   pLevelTriggerDesc = {};
    pLevelTriggerDesc.triggerPosition = { -4.f, 0.f, 20.5 };
    m_pBoss_Trigger = CBoss_Trigger::Create(m_pDevice, m_pContext, &pLevelTriggerDesc);
    if (nullptr == m_pBoss_Trigger)
        return E_FAIL;

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

HRESULT CLevel_SoggySwamp::Ready_Layer_Monster(const _wstring& strLayerTag)
{
    /*CGameObject* pZombie = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Zombie"),
        LEVEL_SOGGYSWAMP, strLayerTag);
    if (nullptr == pZombie)
		return E_FAIL;

    CGameObject* pSkeleton = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Skeleton"),
        LEVEL_SOGGYSWAMP, strLayerTag);
    if (nullptr == pSkeleton)
		return E_FAIL;*/

    CCauldronBoss::CAULDRONBOSS_DESC  cauldronBossDesc = {};
    cauldronBossDesc.slimeCauldronPosition = { 0.45f, 0, 29.8f, 1.f };
    if (nullptr == m_pBoss_Trigger)
        return E_FAIL;
	cauldronBossDesc.bossActivated = m_pBoss_Trigger->Get_Boss_Activated();

    CGameObject* pCauldronBoss = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_CauldronBoss"),
																 LEVEL_SOGGYSWAMP, strLayerTag, &cauldronBossDesc);
    if (nullptr == pCauldronBoss)
        return E_FAIL;

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
