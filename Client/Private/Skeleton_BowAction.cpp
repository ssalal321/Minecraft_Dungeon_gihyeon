#include "Skeleton_BowAction.h"
#include "Skeleton.h"

#include <random>

#include "ArrowPool_Monster.h"
#include "Monster_Arrow.h"

#define SHOOT_ARROW  28.f

CSkeleton_BowAction::CSkeleton_BowAction(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATE_SKELETON_DESC* pDesc)
	: CState_Skeleton(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CSkeleton_BowAction::Init_State()
{
	__super::Init_State();

	m_pSkeleton = dynamic_cast<CSkeleton*>(m_pActor);
	if (nullptr == m_pSkeleton)
        return E_FAIL;

	return S_OK;
}

void CSkeleton_BowAction::State_Enter()
{
	m_bShotArrow = false;

	m_pActorModelCom->Set_Animation(static_cast<_uint>(SKELETON_STATE::BOW_ACTION), true, 0.9f);
}

void CSkeleton_BowAction::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}


void CSkeleton_BowAction::State_Update(_float fTimeDelta)
{
    __super::State_Update(fTimeDelta);
   
    if (m_bAnimationFinished)
    {
        if (Change_State_To_Idle())
            return;

        if (Change_State_To_Walk())
            return;

		if (Change_State_To_HeadSpin())
			return;

		m_bShotArrow = false;
    }

	_float fAnimCurTrackPos = m_pActorModelCom->Get_AnimCurrentTrackPosition();

	if (!m_bShotArrow && SHOOT_ARROW <= fAnimCurTrackPos)
		Shoot_Arrow();

	if (m_bShotArrow)
		return;

	m_PlayerPosition = m_pSkeleton->Get_Player_Position(TEXT("GameObject_Player"),
														m_pGameInstance->Get_CurrentLevelIndex());

	m_pTransformCom->LookAt(XMLoadFloat4(&m_PlayerPosition));
}

void CSkeleton_BowAction::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CSkeleton_BowAction::State_Exit()
{
}

void CSkeleton_BowAction::Collision_Enter(CCollider* pOther)
{
    __super::Collision_Enter(pOther);

	Change_State_To_GetHit();
}

void CSkeleton_BowAction::Collision_Stay(CCollider* pOther)
{
    __super::Collision_Stay(pOther);
}

void CSkeleton_BowAction::Collision_Exit(CCollider* pOther)
{
    __super::Collision_Exit(pOther);
}

void CSkeleton_BowAction::Shoot_Arrow()
{
	CMonster_Arrow*  pMonsterArrow = m_pArrowPool_Monster->Get_Arrow(m_pMonsterInfo->Get_DealPoint());
	if (nullptr == pMonsterArrow)
		return;

	m_bShotArrow = true;

	_float4	 monsterPos = {};
	XMStoreFloat4(&monsterPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	pMonsterArrow->Shoot(monsterPos, m_PlayerPosition);
}

CState_Monster* CSkeleton_BowAction::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATE_SKELETON_DESC* pDesc)
{
    CSkeleton_BowAction* pGameInstance = new CSkeleton_BowAction(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CSkeleton_BowAction");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CSkeleton_BowAction::Free()
{
	__super::Free();
}
