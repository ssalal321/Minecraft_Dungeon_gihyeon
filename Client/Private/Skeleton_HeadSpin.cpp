#include "Skeleton_HeadSpin.h"
#include "Skeleton.h"

CSkeleton_HeadSpin::CSkeleton_HeadSpin(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_Skeleton(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CSkeleton_HeadSpin::Init_State()
{
	__super::Init_State();

	m_pSkeleton = dynamic_cast<CSkeleton*>(m_pActor);
	if (nullptr == m_pSkeleton)
		return E_FAIL;

	return S_OK;
}

void CSkeleton_HeadSpin::State_Enter()
{
	m_pActorModelCom->Set_Animation(static_cast<_uint>(SKELETON_STATE::HEAD_SPIN), false);
}

void CSkeleton_HeadSpin::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CSkeleton_HeadSpin::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (Change_State_To_Attack())
		return;

	if (Change_State_To_Walk())
		return;

	if (m_bAnimationFinished)
	{
		m_pSkeleton->Change_State(Make_SkeletonState(SKELETON_STATE::IDLE));
	}
}

void CSkeleton_HeadSpin::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CSkeleton_HeadSpin::State_Exit()
{
}

void CSkeleton_HeadSpin::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);

	Change_State_To_GetHit(pOther);
}

void CSkeleton_HeadSpin::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CSkeleton_HeadSpin::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Monster* CSkeleton_HeadSpin::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
{
	CSkeleton_HeadSpin* pGameInstance = new CSkeleton_HeadSpin(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CSkeleton_HeadSpin");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CSkeleton_HeadSpin::Free()
{
	__super::Free();
}
