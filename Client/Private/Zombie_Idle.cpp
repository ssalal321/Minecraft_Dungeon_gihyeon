#include "Zombie_Idle.h"

#include <random>

#include "Zombie.h"

CZombie_Idle::CZombie_Idle(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_Zombie(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CZombie_Idle::Init_State()
{
	__super::Init_State();

	m_pZombie = dynamic_cast<CZombie*>(m_pActor);
	if (nullptr == m_pZombie)
		return E_FAIL;

	return S_OK;
}

void CZombie_Idle::State_Enter()
{
	m_fLoopTime = 0.f;

	m_pActorModelCom->Set_Animation(static_cast<_uint>(ZOMBIE_STATE::IDLE), true);
}

void CZombie_Idle::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
	
}

void CZombie_Idle::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (Change_State_To_Attack())
		return;

	if (Change_State_To_Walk())
		return;

	m_fLoopTime += fTimeDelta;

	if (m_fLoopTime >= 1.f)
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		static std::uniform_real_distribution<_float> dist(0.0f, 1.0f); // 0.0 ~ 1.0

		_float  chance = dist(gen);

		if (chance < 0.25f) // 0.0 ~ 0.4 (40%)
		{
			m_pZombie->Change_State(Make_ZombieState(ZOMBIE_STATE::NOVELTY_SLEEP));
			return;
		}
		else if (chance < 0.7f) // 0.4 ~ 0.7 (30%)
		{
			m_pZombie->Change_State(Make_ZombieState(ZOMBIE_STATE::WALK));
			return;
		}
		else // 0.7 ~ 1.0 (30%)
		{
			m_fLoopTime = 0.f; // 아무 상태 변화 없이 시간 초기화
		}
	}

}

void CZombie_Idle::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CZombie_Idle::State_Exit()
{
}

void CZombie_Idle::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);

	Change_State_To_GetHit(pOther);
}

void CZombie_Idle::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CZombie_Idle::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Monster* CZombie_Idle::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
{
	CZombie_Idle* pGameInstance = new CZombie_Idle(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CZombie_Idle");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CZombie_Idle::Free()
{
	__super::Free();
}
