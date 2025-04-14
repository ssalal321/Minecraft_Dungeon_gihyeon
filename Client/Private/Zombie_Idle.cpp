#include "Zombie_Idle.h"

#include <random>

#include "Zombie.h"

CZombie_Idle::CZombie_Idle(CGameObject* pActor, CModel* pZombieModelCom, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
	CTransform* pTransformCom, CNavigation* pNavigationCom)
	: CState_Zombie(pActor, pZombieModelCom, pGameObjectDesc, pTransformCom, pNavigationCom)
{
}

HRESULT CZombie_Idle::Init_State()
{
	__super::Init_State();

	m_pZombie = dynamic_cast<CZombie*>(m_pActor);

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

	m_fLoopTime += fTimeDelta;

	if (m_fLoopTime >= 3.5f)
	{
		// 난수 생성기 및 분포 정의 (정적: 최초 1회만 생성됨)
		static std::random_device rd;
		static std::mt19937 gen(rd());
		static std::uniform_real_distribution<float> dist(0.0f, 1.0f); // 0.0 ~ 1.0 float 확률

		if (dist(gen) < 0.4f) // 40% 확률
		{
			m_pZombie->Change_State(ZOMBIE_STATE::NOVELTY_SLEEP);
			return;
		}
		else
		{
			m_fLoopTime = 0.f;
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

CState_Monster* CZombie_Idle::Create(CGameObject* pActor, CModel* pZombieModelCom, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
							 CTransform* pTransformCom, CNavigation* pNavigationCom)
{
	CZombie_Idle* pGameInstance = new CZombie_Idle(pActor, pZombieModelCom, pGameObjectDesc, pTransformCom, pNavigationCom);

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
