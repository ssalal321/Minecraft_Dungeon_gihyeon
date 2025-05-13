#include "Slime_Large_Idle.h"
#include "State_Slime_Large.h"
#include "Slime_Large.h"

#include <random>

CSlime_Large_Idle::CSlime_Large_Idle(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_Slime_Large(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CSlime_Large_Idle::Init_State()
{
	__super::Init_State();

	m_pSlime_Large = dynamic_cast<CSlime_Large*>(m_pActor);
	if (nullptr == m_pSlime_Large)
		return E_FAIL;

	return S_OK;
}

void CSlime_Large_Idle::State_Enter()
{
	m_fLoopTime = 0.f;

	m_pActorModelCom->Set_Animation(static_cast<_uint>(SLIME_LARGE_STATE::IDLE), true);
}

void CSlime_Large_Idle::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
	
}

void CSlime_Large_Idle::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (Change_State_To_Attack())
		return;

	if (Change_State_To_Walk())
		return;

	m_fLoopTime += fTimeDelta;

	if (m_fLoopTime >= 1.5f)
	{
		// 난수 생성기 및 분포 정의 (정적: 최초 1회만 생성됨)
		static std::random_device rd;
		static std::mt19937 gen(rd());
		static std::uniform_real_distribution<_float> dist(0.0f, 1.0f); // 0.0 ~ 1.0 float 확률

		if (dist(gen) < 0.7f) // 70% 확률
		{
			m_pSlime_Large->Change_State(Make_Slime_LargeState(SLIME_LARGE_STATE::WALK));
			return;
		}
		else
		{
			m_fLoopTime = 0.f;
		}
	}
}

void CSlime_Large_Idle::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CSlime_Large_Idle::State_Exit()
{
}

void CSlime_Large_Idle::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);

	Change_State_To_Stun(pOther);
}

void CSlime_Large_Idle::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CSlime_Large_Idle::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Monster* CSlime_Large_Idle::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
{
	CSlime_Large_Idle* pGameInstance = new CSlime_Large_Idle(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CSlime_Large_Idle");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CSlime_Large_Idle::Free()
{
	__super::Free();
}
