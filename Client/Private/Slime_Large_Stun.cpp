#include "Slime_Large_Stun.h"
#include "Slime_Large.h"


CSlime_Large_Stun::CSlime_Large_Stun(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_Slime_Large(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CSlime_Large_Stun::Init_State()
{
	__super::Init_State();

	m_pSlime_Large = dynamic_cast<CSlime_Large*>(m_pActor);
	if (nullptr == m_pSlime_Large)
		return E_FAIL;

	return S_OK;
}

void CSlime_Large_Stun::State_Enter()
{
	m_pActorModelCom->Set_Animation(static_cast<_uint>(SLIME_LARGE_STATE::STUN), false, 1.5f);
}

void CSlime_Large_Stun::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CSlime_Large_Stun::State_Update(_float fTimeDelta)
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

void CSlime_Large_Stun::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CSlime_Large_Stun::State_Exit()
{
}

void CSlime_Large_Stun::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);

	Modify_HP(pOther);
}

void CSlime_Large_Stun::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CSlime_Large_Stun::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Monster* CSlime_Large_Stun::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
{
	CSlime_Large_Stun* pGameInstance = new CSlime_Large_Stun(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CSlime_Large_Stun");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CSlime_Large_Stun::Free()
{
	__super::Free();
}
