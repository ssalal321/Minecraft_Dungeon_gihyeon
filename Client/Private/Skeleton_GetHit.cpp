#include "Skeleton_GetHit.h"
#include "Skeleton.h"

CSkeleton_GetHit::CSkeleton_GetHit(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATE_SKELETON_DESC* pDesc)
	: CState_Skeleton(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CSkeleton_GetHit::Init_State()
{
	__super::Init_State();

	m_pSkeleton = dynamic_cast<CSkeleton*>(m_pActor);
	if (nullptr == m_pSkeleton)
		return E_FAIL;

	return S_OK;
}

void CSkeleton_GetHit::State_Enter()
{
	m_pActorModelCom->Set_Animation(static_cast<_uint>(SKELETON_STATE::GET_HIT_FRONT), false, 1.5f);
}

void CSkeleton_GetHit::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CSkeleton_GetHit::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (m_bAnimationFinished)
	{
		/*if (Change_State_To_Idle())
			return;*/

		if (Change_State_To_Attack())
			return;

		if (Change_State_To_Walk())
			return;
	}
}

void CSkeleton_GetHit::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CSkeleton_GetHit::State_Exit()
{
}

void CSkeleton_GetHit::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);
}

void CSkeleton_GetHit::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CSkeleton_GetHit::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Monster* CSkeleton_GetHit::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATE_SKELETON_DESC* pDesc)
{
	CSkeleton_GetHit* pGameInstance = new CSkeleton_GetHit(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CSkeleton_GetHit");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CSkeleton_GetHit::Free()
{
	__super::Free();
}
