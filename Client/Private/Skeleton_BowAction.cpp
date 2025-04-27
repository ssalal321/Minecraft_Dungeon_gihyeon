#include "Skeleton_BowAction.h"
#include "Skeleton.h"

#include <random>

#define  AIMING_TIME 0.5f

CSkeleton_BowAction::CSkeleton_BowAction(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_Skeleton(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CSkeleton_BowAction::Init_State()
{
	__super::Init_State();

	m_pSkeleton = dynamic_cast<CSkeleton*>(m_pActor);
	if (nullptr == m_pSkeleton)
        return E_FAIL;

	return S_OK;
}

void CSkeleton_BowAction::State_Enter()
{
	m_fAnimTimer = 0.f;
	m_bArrowShot = false;

	m_pActorModelCom->Set_Animation(static_cast<_uint>(SKELETON_STATE::BOW_ACTION), true);
}

void CSkeleton_BowAction::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}


void CSkeleton_BowAction::State_Update(_float fTimeDelta)
{
    __super::State_Update(fTimeDelta);

    m_fAnimTimer += fTimeDelta;

	if (m_fAnimTimer <= AIMING_TIME)
	{
		_float4 playerPos = m_pSkeleton->Get_Player_Position(TEXT("GameObject_Player"),
															 m_pGameInstance->Get_CurrentLevelIndex());
		m_pTransformCom->LookAt(XMLoadFloat4(&playerPos));
	}

	if (!m_bArrowShot && m_fAnimTimer > AIMING_TIME )
	{
		// 화살 날리기
		// 화살 오브젝트는 본인이 그 방향으로 날아가는 기능을 가지게 한다.

		m_bArrowShot = true;
	}

   
    if (m_bAnimationFinished)
    {
        if (Change_State_To_Idle())
            return;

        if (Change_State_To_Walk())
            return;

		static std::random_device rd;
		static std::mt19937 gen(rd());
		static uniform_real_distribution<float> dist(0.0f, 1.0f); // 0.0 ~ 1.0 float 확률

		if (dist(gen) < 0.3f) // 30% 확률
		{
			m_pSkeleton->Change_State(Make_SkeletonState(SKELETON_STATE::HEAD_SPIN));
			return;
		}

		m_fAnimTimer = 0.f;
		m_bArrowShot = false;
		
    }    
}


void CSkeleton_BowAction::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CSkeleton_BowAction::State_Exit()
{
}

void CSkeleton_BowAction::Collision_Enter(CCollider* pOther)
{
    __super::Collision_Enter(pOther);

	Change_State_To_GetHit(pOther);
}

void CSkeleton_BowAction::Collision_Stay(CCollider* pOther)
{
    __super::Collision_Stay(pOther);
}

void CSkeleton_BowAction::Collision_Exit(CCollider* pOther)
{
    __super::Collision_Exit(pOther);
}

CState_Monster* CSkeleton_BowAction::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
{
    CSkeleton_BowAction* pGameInstance = new CSkeleton_BowAction(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CSkeleton_BowAction");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CSkeleton_BowAction::Free()
{
	__super::Free();
}
