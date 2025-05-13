#include "Skeleton_Idle.h"

#include <random>
#include "Skeleton.h"


CSkeleton_Idle::CSkeleton_Idle(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATE_SKELETON_DESC* pDesc)
	: CState_Skeleton(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CSkeleton_Idle::Init_State()
{
	__super::Init_State();

	m_pSkeleton = dynamic_cast<CSkeleton*>(m_pActor);
	if (nullptr == m_pSkeleton)
		return E_FAIL;

	return S_OK;
}

void CSkeleton_Idle::State_Enter()
{
	m_fLoopTime = 0.f;

	m_pActorModelCom->Set_Animation(static_cast<_uint>(SKELETON_STATE::IDLE), true);
}

void CSkeleton_Idle::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CSkeleton_Idle::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (Change_State_To_Attack())
		return;

	if (Change_State_To_Walk())
		return;

	m_fLoopTime += fTimeDelta;

	if (m_fLoopTime >= 3.5f)
	{
		// 난수 생성기 및 분포 정의 (정적: 최초 1회만 생성됨)
		static std::random_device rd;
		static std::mt19937 gen(rd());
		static std::uniform_real_distribution<float> dist(0.0f, 1.0f); // 0.0 ~ 1.0 float 확률

		if (dist(gen) < 0.4f) // 40% 확률
		{
			m_pSkeleton->Change_State(Make_SkeletonState(SKELETON_STATE::HEAD_SPIN));
			return;
		}
		else
		{
			m_fLoopTime = 0.f;
		}
	}
}

void CSkeleton_Idle::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CSkeleton_Idle::State_Exit()
{
}

void CSkeleton_Idle::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);

	Change_State_To_GetHit(pOther);
}

void CSkeleton_Idle::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CSkeleton_Idle::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Monster* CSkeleton_Idle::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATE_SKELETON_DESC* pDesc)
{
	CSkeleton_Idle* pGameInstance = new CSkeleton_Idle(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CSkeleton_Idle");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CSkeleton_Idle::Free()
{
	__super::Free();
}
