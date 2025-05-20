#include "State_Vindicator.h"
#include "Vindicator.h"

#include "Item.h"
#include "Player_Arrow.h"


CState_Vindicator::CState_Vindicator(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_Monster(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CState_Vindicator::Init_State()
{
	__super::Init_State();

	return S_OK;
}

void CState_Vindicator::State_Enter()
{
}

void CState_Vindicator::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);

	// 체력 0이면 죽엇
}

void CState_Vindicator::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (Change_State_To_Dead())
		return;
}

void CState_Vindicator::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CState_Vindicator::State_Exit()
{
}

void CState_Vindicator::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);

//#ifdef DEBUG
//	_wstring other = pOther->Get_ColliderTag();
//	std::wcerr << "[좀비와 " << other << " 충돌 Enter]" << std::endl;
//#endif
}

void CState_Vindicator::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);

	/*_wstring other = pOther->Get_CollidergGroupTag();

	std::wcerr << "[좀비와 " << other << " 충돌 Stay]" << std::endl;*/
}

void CState_Vindicator::Collision_Exit(CCollider* pOther)
{
	/*_wstring other = pOther->Get_CollidergGroupTag();

	std::wcerr << "[좀비와 " << other << " 충돌 Exit]" << std::endl;*/
}

_bool CState_Vindicator::Change_State_To_Attack()
{
	// 공격 가능 거리 && 스턴 X 상태
	_float lengthToPlayer = m_pVindicator->Length_To_Player();

	if (lengthToPlayer < m_pMonsterInfo->fAttackableRange)
	{
		m_pVindicator->Change_State(Make_VindicatorState(VINDICATOR_STATE::ATTACK));
		return true;
	}

	return false;
}

_bool CState_Vindicator::Change_State_To_Walk()
{
	_float lengthToPlayer = m_pVindicator->Length_To_Player();

	// 플레이어 인지 거리
	if (m_pVindicator->Player_In_DetectRange()/* &&
		lengthToPlayer > m_pMonsterInfo->fAttackableRange*/)
	{
		m_pVindicator->Change_State(Make_VindicatorState(VINDICATOR_STATE::WALK));
		return true;
	}

	return false;
}

_bool CState_Vindicator::Change_State_To_Idle()
{
	_bool	playerInRange = m_pVindicator->Player_In_DetectRange();

	if (!playerInRange)
	{
		m_pVindicator->Change_State(Make_VindicatorState(VINDICATOR_STATE::IDLE));
		return true;
	}

	return false;
}

_bool CState_Vindicator::Change_State_To_Dead()
{
	if (m_pMonsterInfo->Get_CurrentHP() <= 0)
	{
		m_pVindicator->Change_State(Make_VindicatorState(VINDICATOR_STATE::DEAD));
		return true;
	}

	return false;
}

_bool CState_Vindicator::Modify_HP(CCollider* pOther)
{
	if (TEXT("Player_Weapon") == pOther->Get_ColliderTag()
		&& pOther->Get_Other_Collision_Activated())
	{
		CItem* pItem = dynamic_cast<CItem*>(pOther->Get_OwnerObject());
		m_pMonsterInfo->Modify_CurrentHp(-pItem->Get_DealPoint());

		return true;
	}

	if (TEXT("Player_Arrow") == pOther->Get_ColliderTag()
		&& pOther->Get_Other_Collision_Activated())
	{
		CPlayer_Arrow* pPlayerArrow = dynamic_cast<CPlayer_Arrow*>(pOther->Get_OwnerObject());
		m_pMonsterInfo->Modify_CurrentHp(-pPlayerArrow->Get_DealPoint());

		return true;
	}

	return false;
}


void CState_Vindicator::Free()
{
	__super::Free();
}