#include "CauldronBoss_BasicAttack.h"

#include <iostream>
#include <ostream>

#include "BabyZombie.h"
#include "CauldronBoss.h"
#include "Vindicator.h"

#define ATTACKSTART 7.f
#define ATTACKFINISH 13.f

CCauldronBoss_BasicAttack::CCauldronBoss_BasicAttack(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_CauldronBoss(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CCauldronBoss_BasicAttack::Init_State()
{
	__super::Init_State();

	m_pCauldronBoss = dynamic_cast<CCauldronBoss*>(m_pActor);
	if (nullptr == m_pCauldronBoss)
        return E_FAIL;

	return S_OK;
}

void CCauldronBoss_BasicAttack::State_Enter()
{
    m_bSummoned = false;

	m_pActorModelCom->Set_Animation(static_cast<_uint>(CAULDRONBOSS_STATE::BASIC_ATTACK), false, 1.f);

    std::cerr << "[CauldronBoss_BasicAttack]" << std::endl;
}

void CCauldronBoss_BasicAttack::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CCauldronBoss_BasicAttack::State_Update(_float fTimeDelta)
{
    __super::State_Update(fTimeDelta);

    _float4 playerPos = m_pCauldronBoss->Get_Player_Position(TEXT("GameObject_Player"),
															 m_pGameInstance->Get_CurrentLevelIndex());

    if (!m_bSummoned)
    {
        //Spawn_Monsters(playerPos);
        m_bSummoned = true;
    }

    if (m_bAnimationFinished)
    {
        if (Change_State_To_Idle())
            return;
    }

    m_pTransformCom->LookAt(XMLoadFloat4(&playerPos));
}


void CCauldronBoss_BasicAttack::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CCauldronBoss_BasicAttack::State_Exit()
{
}

void CCauldronBoss_BasicAttack::Collision_Enter(CCollider* pOther)
{
    __super::Collision_Enter(pOther);

    Modify_HP(pOther);

	if (Change_State_To_IntenseSpin(pOther))
		return;
}

void CCauldronBoss_BasicAttack::Collision_Stay(CCollider* pOther)
{
    __super::Collision_Stay(pOther);
}

void CCauldronBoss_BasicAttack::Collision_Exit(CCollider* pOther)
{
    __super::Collision_Exit(pOther);
}

void CCauldronBoss_BasicAttack::Spawn_Monsters(const _float4& playerPos)
{
    _float4 bossPos;
    XMStoreFloat4(&bossPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
    bossPos.y += 3.f; // 困肺 剁快扁

    // BabyZombie 2付府 积己
    for (_int i = 0; i < 2; ++i)
    {
        _float angle = static_cast<_float>(rand() % 360);
        _float radius = 3.f + static_cast<_float>(rand() % 3);

        _vector vOffset = XMVectorSet(radius * cosf(XMConvertToRadians(angle)), 0.f, radius * sinf(XMConvertToRadians(angle)), 0.f);
        _vector vLandingPos = XMLoadFloat4(&playerPos) + vOffset;

        CBabyZombie::BABYZOMBIE_DESC desc{};
        desc.babyZombiePosition = bossPos;
        desc.currentCellIndex = m_pNavigationCom->Get_CurrentCellIndex();

        CGameObject* pZombieObj = m_pGameInstance->Add_GameObject(
            LEVEL_STATIC, TEXT("Prototype_GameObject_BabyZombie"),
            m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Monster"), &desc);

        if (pZombieObj)
        {
            CMonster* pMonster = dynamic_cast<CMonster*>(pZombieObj);
            pMonster->Set_Can_be_Eaten(true, m_pCauldronBoss);
            pMonster->Jump_To_Target(vLandingPos);

            CNavigation* pNavigation = dynamic_cast<CNavigation*>(pMonster->Find_Component(TEXT("Com_Navigation")));
            pNavigation->Lock_Cell(899);
            pNavigation->Lock_Cell(900);
        }        
    }

    // Vindicator 1付府 积己
    _float angle = static_cast<_float>(rand() % 360);
    _float radius = 3.f + static_cast<_float>(rand() % 3);
    _vector vOffset = XMVectorSet(radius * cosf(XMConvertToRadians(angle)), 0.f, radius * sinf(XMConvertToRadians(angle)), 0.f);
    _vector vLandingPos = XMLoadFloat4(&playerPos) + vOffset;

    CVindicator::VINDICATOR_DESC vDesc{};
    vDesc.vindicatorPosition = bossPos;
    vDesc.currentCellIndex = m_pNavigationCom->Get_CurrentCellIndex();

    CGameObject* pVindicatorObj = m_pGameInstance->Add_GameObject(
        LEVEL_STATIC, TEXT("Prototype_GameObject_Vindicator"),
        m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Monster"), &vDesc);

    if (pVindicatorObj)
    {
        CMonster* pMonster = dynamic_cast<CMonster*>(pVindicatorObj);
        pMonster->Set_Can_be_Eaten(true, m_pCauldronBoss);
        pMonster->Jump_To_Target(vLandingPos);

        CNavigation* pNavigation = dynamic_cast<CNavigation*>(pMonster->Find_Component(TEXT("Com_Navigation")));
        pNavigation->Lock_Cell(899);
        pNavigation->Lock_Cell(900);
    }
}


CState_Monster* CCauldronBoss_BasicAttack::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
{
	CCauldronBoss_BasicAttack* pGameInstance = new CCauldronBoss_BasicAttack(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CCauldronBoss_BasicAttack");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CCauldronBoss_BasicAttack::Free()
{
	__super::Free();
}
