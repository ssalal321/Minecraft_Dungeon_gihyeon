#include "Skeleton_Walk.h"

#include "Player.h"
#include "Skeleton.h"

CSkeleton_Walk::CSkeleton_Walk(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATE_SKELETON_DESC* pDesc)
	: CState_Skeleton(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CSkeleton_Walk::Init_State()
{
	__super::Init_State();

	m_pSkeleton = dynamic_cast<CSkeleton*>(m_pActor);
	if (nullptr == m_pSkeleton)
		return E_FAIL;

	return S_OK;
}

void CSkeleton_Walk::State_Enter()
{
	m_bRetreating = false;
	m_vOppositeDir = { 0.f, 0.f, 0.f, 0.f };
	m_pActorModelCom->Set_Animation(static_cast<_uint>(SKELETON_STATE::WALK), true);
}

void CSkeleton_Walk::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CSkeleton_Walk::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (Change_State_To_Idle())
		return;

	if (Change_State_To_Attack())
		return;

	_uint  currentLevelIndex = m_pGameInstance->Get_CurrentLevelIndex();

	_float4 playerPos = m_pSkeleton->Get_Player_Position(TEXT("GameObject_Player"), currentLevelIndex);
	_vector vecToPlayer = m_pSkeleton->Vec_To_Player(TEXT("GameObject_Player"), currentLevelIndex);
	

	_float lengthToPlayer = {};
	XMStoreFloat(&lengthToPlayer, XMVector3Length(vecToPlayer));

	if (m_pMonsterInfo->fAttackableRange + 1.f < lengthToPlayer)
	{
		m_bRetreating = false;  // 걷는 방향 초기화

		m_pTransformCom->LookAt(XMLoadFloat4(&playerPos));
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
	}
	else if (lengthToPlayer < m_pMonsterInfo->fAttackableRange - 1.f)
	{
		if (!m_bRetreating)
		{
			m_bRetreating = true;
			m_vOppositeDir = XMVector3Normalize(-vecToPlayer);
			m_vRetreatStartPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		}

		_vector vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vTargetPos = vCurPos + m_vOppositeDir;

		m_pTransformCom->LookAt(vTargetPos);
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);

		// 일정 거리 이상 도망쳤으면 공격 상태 전환
		_vector vMoved = vCurPos - m_vRetreatStartPos;
		_float  fMovedDist = {};
		XMStoreFloat(&fMovedDist, XMVector3Length(vMoved));

		if (fMovedDist > 5.f)
		{
			m_pSkeleton->Change_State(Make_SkeletonState(SKELETON_STATE::BOW_ACTION));
			return;
		}
	}
}


void CSkeleton_Walk::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CSkeleton_Walk::State_Exit()
{
}

void CSkeleton_Walk::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);

	Change_State_To_GetHit(pOther);
}

void CSkeleton_Walk::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CSkeleton_Walk::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Monster* CSkeleton_Walk::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATE_SKELETON_DESC* pDesc)
{
	CSkeleton_Walk* pGameInstance = new CSkeleton_Walk(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CSkeleton_Walk");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CSkeleton_Walk::Free()
{
	__super::Free();
}
