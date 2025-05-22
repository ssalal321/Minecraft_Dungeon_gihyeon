#include "State_CauldronBoss.h"
#include "CauldronBoss.h"
#include "GateFence.h"

#include "Player_Arrow.h"
#include "Item.h"
#include "Player.h"
#include "Slime_Small.h"
#include "UIObject.h"


CState_CauldronBoss::CState_CauldronBoss(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_Monster(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CState_CauldronBoss::Init_State()
{
	__super::Init_State();

	return S_OK;
}

void CState_CauldronBoss::State_Enter()
{
}

void CState_CauldronBoss::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);

	// 체력 0이면 죽엇
}

void CState_CauldronBoss::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (m_pMonsterInfo->Get_CurrentHP() <= 0)
	{
		m_pBigColliderCom->Set_ColliderActive(false);
		m_pSmallColliderCom->Set_ColliderActive(false);
		m_pCauldronBoss->Get_My_HPUIObject()->Set_GameObject_Active(false);

		m_pCauldronBoss->Find_PartObject(TEXT("Part_Body"))->Set_Dying(true);

		CGameObject* pGateFence = m_pGameInstance->Find_GameObject(TEXT("GameObject_GateFence_0"),
			m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_BackGround"));
		dynamic_cast<CGateFence*>(pGateFence)->Set_Dying(true);

		CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject(TEXT("GameObject_Player"), m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Player")));
		CNavigation* pPlayerNav = dynamic_cast<CNavigation*>(pPlayer->Find_Component(TEXT("Com_Navigation")));
		pPlayerNav->Unlock_Cell(899);
		pPlayerNav->Unlock_Cell(900);

		Change_State_To_Idle();
		return;
	}

}

void CState_CauldronBoss::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CState_CauldronBoss::State_Exit()
{
}

void CState_CauldronBoss::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);
}

void CState_CauldronBoss::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CState_CauldronBoss::Collision_Exit(CCollider* pOther)
{
}

_bool CState_CauldronBoss::Change_State_To_BasicAttack()
{
	// 공격 가능 거리 && 스턴 X 상태
	m_pCauldronBoss->Change_State(Make_CauldronBossState(CAULDRONBOSS_STATE::BASIC_ATTACK));
	return true;
}

_bool CState_CauldronBoss::Change_State_To_StrongAttack()
{
	m_pCauldronBoss->Change_State(Make_CauldronBossState(CAULDRONBOSS_STATE::STRONG_ATTACK));
	return true;
}

_bool CState_CauldronBoss::Change_State_To_IntenseSpin(CCollider* pOther)
{
	// 부딪혔을 때
	if (TEXT("Monster_Body_Small") == pOther->Get_ColliderTag())
	{
		// 죽은 몬스터라면
		CMonster* pCurrMonster = dynamic_cast<CMonster*>(dynamic_cast<CPartObject*>(pOther->Get_OwnerObject())->Get_ContainerObject());
		if (pCurrMonster->Get_MonsterInfo()->Get_CurrentHP() <= 0)
		{
			// 흡수할 거야!!
			m_pCauldronBoss->Change_State(Make_CauldronBossState(CAULDRONBOSS_STATE::INTENSE_SPIN));
			return true;
		}
	}

	return false;
}

_bool CState_CauldronBoss::Change_State_To_Idle()
{
	m_pCauldronBoss->Change_State(Make_CauldronBossState(CAULDRONBOSS_STATE::IDLE));
	return true;
}

_bool CState_CauldronBoss::Modify_HP(CCollider* pOther)
{
	if (TEXT("Player_Weapon") == pOther->Get_ColliderTag()
		&& pOther->Get_Other_Collision_Activated())
	{
		CItem* pItem = dynamic_cast<CItem*>(pOther->Get_OwnerObject());
		m_pMonsterInfo->Modify_CurrentHp(-pItem->Get_DealPoint());

		m_pCauldronBoss->Render_DamageFont(pItem->Get_DealPoint(), 7.f);

		return true;
	}

	if (TEXT("Player_Arrow") == pOther->Get_ColliderTag()
		&& pOther->Get_Other_Collision_Activated())
	{
		CPlayer_Arrow* pPlayerArrow = dynamic_cast<CPlayer_Arrow*>(pOther->Get_OwnerObject());
		m_pMonsterInfo->Modify_CurrentHp(-pPlayerArrow->Get_DealPoint());

		m_pCauldronBoss->Render_DamageFont(pPlayerArrow->Get_DealPoint(), 7.f);

		return true;
	}

	return false;
}

void CState_CauldronBoss::Free()
{
	__super::Free();
}