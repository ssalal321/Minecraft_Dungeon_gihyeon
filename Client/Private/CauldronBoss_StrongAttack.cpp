#include "CauldronBoss_StrongAttack.h"

#include <iostream>
#include <ostream>

#include "State_CauldronBoss.h"
#include "CauldronBoss.h"

#include <random>

#include "Player.h"
#include "Slime_Cauldron.h"

CCauldronBoss_StrongAttack::CCauldronBoss_StrongAttack(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_CauldronBoss(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CCauldronBoss_StrongAttack::Init_State()
{
	__super::Init_State();

	m_pCauldronBoss = dynamic_cast<CCauldronBoss*>(m_pActor);
	if (nullptr == m_pCauldronBoss)
		return E_FAIL;

	m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject(TEXT("GameObject_Player"), m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Player")));
	if (!m_pPlayer)
		return E_FAIL;

	return S_OK;
}

void CCauldronBoss_StrongAttack::State_Enter()
{
	m_bShot = false;

	m_pActorModelCom->Set_Animation(static_cast<_uint>(CAULDRONBOSS_STATE::STRONG_ATTACK), false, 1.1f);

    std::cerr << "[CauldronBoss_StrongAttack]" << std::endl;
}

void CCauldronBoss_StrongAttack::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CCauldronBoss_StrongAttack::State_Update(_float fTimeDelta)
{
    __super::State_Update(fTimeDelta);

    // 플레이어 위치를 기준으로 랜덤하게 발사할 위치를 계산
    _float4  playerPos = m_pCauldronBoss->Get_Player_Position(TEXT("GameObject_Player"), m_pGameInstance->Get_CurrentLevelIndex());

    if (!m_bShot)
    {
        // 슬라임 발사
        for (_int i = 0; i < 0; ++i)  // 한 번에 3마리 발사
        {
            // 랜덤한 방향으로 발사
            _float angle = static_cast<_float>(rand() % 360);  // 0~360도 범위에서 랜덤 각도 생성
            _float radius = 3.0f + rand() % 3;  // 플레이어 주변 3~5 범위 내에서 랜덤 거리 생성

            _vector vOffset = XMVectorSet(radius * cosf(XMConvertToRadians(angle)), 0.0f, radius * sinf(XMConvertToRadians(angle)), 0.0f);
            _vector vLandingPosition = XMLoadFloat4(&playerPos) + vOffset;


            CSlime_Cauldron::SLIME_CAULDRON_DESC    slimeCauldronDesc = {};
            _float4     cauldronBossPos;
            XMStoreFloat4(&cauldronBossPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
            cauldronBossPos.y += 3.f;

            slimeCauldronDesc.slimeCauldronPosition = cauldronBossPos;

            // 슬라임 생성
            CGameObject* pGameObject = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Slime_Cauldron"),
														m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Monster"), &slimeCauldronDesc);
            if (nullptr == pGameObject)
                return;

            CSlime_Cauldron*  pSlimeCauldron = dynamic_cast<CSlime_Cauldron*>(pGameObject);
         
        	// 목표 위치로 점프 시작 신호 보내기
        	pSlimeCauldron->Jump_To_Target(vLandingPosition);
            
        }

        m_bShot = true;
    }

    if (m_bAnimationFinished)
    {
        if (Change_State_To_Idle())
            return;
    }

    m_pTransformCom->LookAt(XMLoadFloat4(&playerPos));
}


void CCauldronBoss_StrongAttack::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CCauldronBoss_StrongAttack::State_Exit()
{
}

void CCauldronBoss_StrongAttack::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);

	Modify_HP(pOther);

	if (Change_State_To_IntenseSpin(pOther))
		return;
}

void CCauldronBoss_StrongAttack::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CCauldronBoss_StrongAttack::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Monster* CCauldronBoss_StrongAttack::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
{
	CCauldronBoss_StrongAttack* pGameInstance = new CCauldronBoss_StrongAttack(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CCauldronBoss_StrongAttack");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CCauldronBoss_StrongAttack::Free()
{
	__super::Free();
}
