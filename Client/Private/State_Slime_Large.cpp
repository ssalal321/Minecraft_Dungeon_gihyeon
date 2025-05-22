#include "State_Slime_Large.h"
#include "Slime_Large.h"

#include "Player_Arrow.h"
#include "Item.h"
#include "MonsterRush_Trigger.h"
#include "Slime_Medium.h"


CState_Slime_Large::CState_Slime_Large(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_Monster(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CState_Slime_Large::Init_State()
{
	__super::Init_State();

	return S_OK;
}

void CState_Slime_Large::State_Enter()
{
}

void CState_Slime_Large::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);

	// 체력 0이면 죽엇
}

void CState_Slime_Large::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (m_pMonsterInfo->Get_CurrentHP() <= 0)
	{
		if (nullptr != m_pSlime_Large->Get_MonsterRush_Trigger())
			m_pSlime_Large->Get_MonsterRush_Trigger()->Notify_Monster_Died(m_pSlime_Large);

		m_pActor->Set_GameObject_Active(false);
		m_pBigColliderCom->Set_ColliderActive(false);
		m_pSmallColliderCom->Set_ColliderActive(false);
	}

	if (false == m_pActor->Get_GameObject_Active())
	{
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);  // 죽은 위치

		
		_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		vRight = XMVector3Normalize(vRight);  // 정규화
		_vector vOffset = vRight * 1.3f;      // 1.3f 만큼 양옆으로 떨어지게

		// Slime_Medium 생성할 두 위치 계산
		_vector  vLeftPos  = vPosition - vOffset;
		_vector  vRightPos = vPosition + vOffset;

		// 4. float4로 변환해서 desc에 넣기
		CSlime_Medium::SLIME_MEDIUM_DESC  leftDesc{};
		XMStoreFloat4(&leftDesc.slimeMediumPosition, vLeftPos);
		leftDesc.currentCellIndex = m_pNavigationCom->Get_CurrentCellIndex();

		CGameObject* pSlimeMedium0 = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Slime_Medium"),
															m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Monster"), &leftDesc);

		if (nullptr != m_pSlime_Large->Get_MonsterRush_Trigger())
		{
			CMonster* pMonster = dynamic_cast<CMonster*>(pSlimeMedium0);
			pMonster->Set_MyRushTrigger(m_pSlime_Large->Get_MonsterRush_Trigger());
			pMonster->Get_MonsterRush_Trigger()->Add_Monster(pMonster);

			CNavigation*	pNavigation = dynamic_cast<CNavigation*>(pMonster->Find_Component(TEXT("Com_Navigation")));
			pNavigation->Lock_Cell(1569);  // 원래는 이것도 trigger에 저장해서 받아와야 하지만 시간 없으므로..
			pNavigation->Lock_Cell(1688);
		}
			

		CSlime_Medium::SLIME_MEDIUM_DESC  rightDesc{};
		XMStoreFloat4(&rightDesc.slimeMediumPosition, vRightPos);
		rightDesc.currentCellIndex = m_pNavigationCom->Get_CurrentCellIndex();

		CGameObject* pSlimeMedium1 = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Slime_Medium"),
															m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Monster"), &rightDesc);

		if (nullptr != m_pSlime_Large->Get_MonsterRush_Trigger())
		{
			CMonster* pMonster = dynamic_cast<CMonster*>(pSlimeMedium1);
			pMonster->Set_MyRushTrigger(m_pSlime_Large->Get_MonsterRush_Trigger());
			m_pSlime_Large->Get_MonsterRush_Trigger()->Add_Monster(pMonster);

			CNavigation*	pNavigation = dynamic_cast<CNavigation*>(pMonster->Find_Component(TEXT("Com_Navigation")));
			pNavigation->Lock_Cell(1569);  // 원래는 이것도 trigger에 저장해서 받아와야 하지만 시간 없으므로..
			pNavigation->Lock_Cell(1688);
		}
			
	}

}

void CState_Slime_Large::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CState_Slime_Large::State_Exit()
{
}

void CState_Slime_Large::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);
}

void CState_Slime_Large::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CState_Slime_Large::Collision_Exit(CCollider* pOther)
{
}

_bool CState_Slime_Large::Change_State_To_Attack()
{
	// 공격 가능 거리 && 스턴 X 상태
	_float lengthToPlayer = m_pSlime_Large->Length_To_Player();

	if (lengthToPlayer < m_pMonsterInfo->fAttackableRange)
	{
		m_pNavigationCom->SetUp_On_Navigation(m_pTransformCom);
		m_pTransformCom->Set_Is_Jumping(false);

		m_pSlime_Large->Change_State(Make_Slime_LargeState(SLIME_LARGE_STATE::ATTACK));
		return true;
	}

	return false;
}

_bool CState_Slime_Large::Change_State_To_Walk()
{
	_float lengthToPlayer = m_pSlime_Large->Length_To_Player();

	// 플레이어 인지 거리 && 스턴 X 상태
	if (m_pSlime_Large->Player_In_DetectRange() &&
		lengthToPlayer > m_pMonsterInfo->fAttackableRange)
	{
		m_pSlime_Large->Change_State(Make_Slime_LargeState(SLIME_LARGE_STATE::WALK));
		return true;
	}

	return false;
}

_bool CState_Slime_Large::Change_State_To_Idle()
{
	_bool	playerInRange = m_pSlime_Large->Player_In_DetectRange();

	if (!playerInRange)
	{
		m_pSlime_Large->Change_State(Make_Slime_LargeState(SLIME_LARGE_STATE::IDLE));
		return true;
	}

	return false;
}

_bool CState_Slime_Large::Modify_HP(CCollider* pOther)
{
	if (TEXT("Player_Weapon") == pOther->Get_ColliderTag()
		&& pOther->Get_Other_Collision_Activated())
	{
		CItem* pItem = dynamic_cast<CItem*>(pOther->Get_OwnerObject());
		m_pMonsterInfo->Modify_CurrentHp(-pItem->Get_DealPoint());

		m_pSlime_Large->Render_DamageFont(pItem->Get_DealPoint(), 6.f);

		return true;
	}

	if (TEXT("Player_Arrow") == pOther->Get_ColliderTag()
		&& pOther->Get_Other_Collision_Activated())
	{
		CPlayer_Arrow* pPlayerArrow = dynamic_cast<CPlayer_Arrow*>(pOther->Get_OwnerObject());
		m_pMonsterInfo->Modify_CurrentHp(-pPlayerArrow->Get_DealPoint());

		m_pSlime_Large->Render_DamageFont(pPlayerArrow->Get_DealPoint(), 6.f);

		return true;
	}

	return false;
}

_bool CState_Slime_Large::Change_State_To_Stun(CCollider* pOther)
{
	if (Modify_HP(pOther))
	{
		m_pSlime_Large->Change_State(Make_BabyZombieState(BABYZOMBIE_STATE::GET_HIT));
		return true;
	}

	return false;
}

void CState_Slime_Large::Free()
{
	__super::Free();
}