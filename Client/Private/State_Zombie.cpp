#include "State_Zombie.h"

#include "Zombie.h"

CState_Zombie::CState_Zombie(CGameObject* pActor, CModel* pMonsterModelCom, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
                             CTransform* pTransformCom, CNavigation* pNavigationCom)
	: CState_Monster(pActor, pMonsterModelCom, pGameObjectDesc, pTransformCom, pNavigationCom)
{
}

HRESULT CState_Zombie::Init_State()
{
	__super::Init_State();

	return S_OK;
}

void CState_Zombie::State_Enter()
{
}

void CState_Zombie::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);

	// 체력 0이면 죽엇
}

void CState_Zombie::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	_float lengthToPlayer = m_pZombie->Length_To_Player(TEXT("Prototype_GameObject_PlayerHex"), LEVEL_GAMEPLAY);

	// 체력 0이면 죽기

	// 공격 받았을 때 스턴 걸리기

	// 공격 가능 거리 && 스턴 X 상태
	if (lengthToPlayer < m_pMonsterDesc->fAttackableRange && !m_pMonsterDesc->bStunned)
	{
		m_pZombie->Change_State(ZOMBIE_STATE::ATTACK);
		return;
	}

	// 플레이어 인지 거리 && 스턴 X 상태
	if (m_pZombie->Player_In_DetectRange(TEXT("Prototype_GameObject_PlayerHex"), LEVEL_GAMEPLAY) &&
		!m_pMonsterDesc->bStunned)
	{
		m_pZombie->Change_State(ZOMBIE_STATE::WALK);
		return;
	}

	//m_pZombie->Change_State(ZOMBIE_STATE::IDLE);
}

void CState_Zombie::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CState_Zombie::State_Exit()
{
}

void CState_Zombie::Free()
{
	__super::Free();
}
