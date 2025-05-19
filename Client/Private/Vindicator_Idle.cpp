#include "Vindicator_Idle.h"
#include "Vindicator.h"

#include <random>

CVindicator_Idle::CVindicator_Idle(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_Vindicator(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CVindicator_Idle::Init_State()
{
	__super::Init_State();

	m_pVindicator = dynamic_cast<CVindicator*>(m_pActor);
	if (nullptr == m_pVindicator)
		return E_FAIL;

	return S_OK;
}

void CVindicator_Idle::State_Enter()
{
	m_fLoopTime = 0.f;

	m_pActorModelCom->Set_Animation(static_cast<_uint>(VINDICATOR_STATE::IDLE), true, 0.8f);
}

void CVindicator_Idle::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CVindicator_Idle::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (Change_State_To_Attack())
		return;

	if (Change_State_To_Walk())
		return;

	m_fLoopTime += fTimeDelta;

	if (m_fLoopTime >= 2.f)
	{
		// 난수 생성기 및 분포 정의 (정적: 최초 1회만 생성됨)
		static std::random_device rd;
		static std::mt19937 gen(rd());
		static std::uniform_real_distribution<float> dist(0.0f, 1.0f); // 0.0 ~ 1.0 float 확률

		if (dist(gen) < 0.5f) // 50% 확률
		{
			m_pVindicator->Change_State(Make_VindicatorState(VINDICATOR_STATE::NOVELTY));
			return;
		}
		else
		{
			m_fLoopTime = 0.f;
		}
	}
}

void CVindicator_Idle::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CVindicator_Idle::State_Exit()
{
}

void CVindicator_Idle::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);

	Modify_HP(pOther);
}

void CVindicator_Idle::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CVindicator_Idle::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Monster* CVindicator_Idle::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
{
	CVindicator_Idle* pGameInstance = new CVindicator_Idle(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CVindicator_Idle");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CVindicator_Idle::Free()
{
	__super::Free();
}
