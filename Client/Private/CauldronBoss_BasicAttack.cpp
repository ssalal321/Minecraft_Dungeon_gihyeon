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

    _float4 playerPos = m_pCauldronBoss->Get_Player_Position(TEXT("GameObject_Player"),
        m_pGameInstance->Get_CurrentLevelIndex());

    if (!m_bSummoned)
    {
        for (_int i = 0; i < 2; ++i)  // 한 번에 2마리 생성
        {
            // 랜덤 각도 및 거리
            _float angle = static_cast<_float>(rand() % 360);
            _float radius = 3.0f + rand() % 3;

            _vector vOffset = XMVectorSet(radius * cosf(XMConvertToRadians(angle)), 0.0f, radius * sinf(XMConvertToRadians(angle)), 0.0f);
            _vector vLandingPosition = XMLoadFloat4(&playerPos) + vOffset;

            // 생성 정보 설정
            CBabyZombie::BABYZOMBIE_DESC    babyZombieDesc = {};
            _float4     cauldronBossPos;
            XMStoreFloat4(&cauldronBossPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
            cauldronBossPos.y += 3.f;

            babyZombieDesc.babyZombiePosition   = cauldronBossPos;
            babyZombieDesc.currentCellIndex     = m_pNavigationCom->Get_CurrentCellIndex();

            // 몬스터 생성
            CGameObject* pGameObject = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_BabyZombie"),
														  m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Monster"), &babyZombieDesc);
            if (nullptr == pGameObject)
                return;

            // 추가 설정
            CMonster* pMonster = dynamic_cast<CMonster*>(pGameObject);

        	pMonster->Set_Can_be_Eaten(true, m_pCauldronBoss);

            pMonster->Jump_To_Target(vLandingPosition);
        }

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
