#include "BabyZombie_Idle.h"
#include "BabyZombie.h"

#include <random>

CBabyZombie_Idle::CBabyZombie_Idle(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_BabyZombie(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CBabyZombie_Idle::Init_State()
{
	__super::Init_State();

	m_pBabyZombie = dynamic_cast<CBabyZombie*>(m_pActor);
	if (nullptr == m_pBabyZombie)
		return E_FAIL;

	return S_OK;
}

void CBabyZombie_Idle::State_Enter()
{
	m_fLoopTime = 0.f;

	m_pActorModelCom->Set_Animation(static_cast<_uint>(BABYZOMBIE_STATE::IDLE), true, 0.8f);
}

void CBabyZombie_Idle::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CBabyZombie_Idle::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (Change_State_To_Attack())
		return;

	if (Change_State_To_Walk())
		return;

	m_fLoopTime += fTimeDelta;

	if (m_fLoopTime >= 2.5f)
	{
		// 난수 생성기 및 분포 정의 (정적: 최초 1회만 생성됨)
		static std::random_device rd;
		static std::mt19937 gen(rd());
		static std::uniform_real_distribution<float> dist(0.0f, 1.0f); // 0.0 ~ 1.0 float 확률

		if (dist(gen) < 0.4f) // 40% 확률
		{
			m_pBabyZombie->Change_State(Make_BabyZombieState(BABYZOMBIE_STATE::NOVELTY));
			return;
		}
		else
		{
			m_fLoopTime = 0.f;
		}
	}
}

void CBabyZombie_Idle::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CBabyZombie_Idle::State_Exit()
{
}

void CBabyZombie_Idle::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);

	Change_State_To_GetHit(pOther);
}

void CBabyZombie_Idle::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CBabyZombie_Idle::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Monster* CBabyZombie_Idle::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
{
	CBabyZombie_Idle* pGameInstance = new CBabyZombie_Idle(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CBabyZombie_Idle");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CBabyZombie_Idle::Free()
{
	__super::Free();
}
