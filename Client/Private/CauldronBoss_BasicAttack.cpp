#include "CauldronBoss_BasicAttack.h"

#include <iostream>
#include <ostream>

#include "BabyZombie.h"
#include "CauldronBoss.h"

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

    if (m_bAnimationFinished)
    {
        if (Change_State_To_Idle())
            return;
    }

    _float4  playerPos = m_pCauldronBoss->Get_Player_Position(TEXT("GameObject_Player"), m_pGameInstance->Get_CurrentLevelIndex());

	if (!m_bSummoned)
    {
        // 플레이어 위치를 기준으로 랜덤하게 발사할 위치를 계산

        // BabyZombie 생성
        for (_int i = 0; i < 1; ++i)  // 한 번에 1마리 생성
        {
            // 랜덤한 방향으로
            _float angle = static_cast<_float>(rand() % 360);  // 0~360도 범위에서 랜덤 각도 생성
            _float radius = 4.0f + static_cast<_float>(rand() % 3);  // 플레이어 주변 4~6 범위 내에서 랜덤 거리 생성

            _vector   vOffset = XMVectorSet(radius * cosf(XMConvertToRadians(angle)), 0.0f, radius * sinf(XMConvertToRadians(angle)), 0.0f);
            _vector   vSpawnPos = XMLoadFloat4(&playerPos) + vOffset;

            // BabyZombie 생성
            _float4     spawnPosition;
            XMStoreFloat4(&spawnPosition, vSpawnPos);
            CBabyZombie::BABYZOMBIE_DESC   babyZombieDesc = {};
            babyZombieDesc.babyZombiePosition = spawnPosition;
            babyZombieDesc.currentCellIndex = m_pNavigationCom->Get_CurrentCellIndex();

            CGameObject* pGameObject = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_BabyZombie"), m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Monster"), &babyZombieDesc);
            if (nullptr == pGameObject)
                return;

            dynamic_cast<CMonster*>(pGameObject)->Set_Can_be_Eaten(true, m_pCauldronBoss);
        }

        m_bSummoned = true;
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
