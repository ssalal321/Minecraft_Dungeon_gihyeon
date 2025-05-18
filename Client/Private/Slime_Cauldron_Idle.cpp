#include "Slime_Cauldron_Idle.h"
#include "Slime_Cauldron.h"

CSlime_Cauldron_Idle::CSlime_Cauldron_Idle(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATE_SLIME_CAULDRON_DESC* pDesc)
	: CState_Slime_Cauldron(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CSlime_Cauldron_Idle::Init_State()
{
	__super::Init_State();

	m_pSlime_Cauldron = dynamic_cast<CSlime_Cauldron*>(m_pActor);
	if (nullptr == m_pSlime_Cauldron)
		return E_FAIL;

	return S_OK;
}

void CSlime_Cauldron_Idle::State_Enter()
{
	m_pActorModelCom->Set_Animation(static_cast<_uint>(SLIME_CAULDRON_STATE::IDLE), false);
}

void CSlime_Cauldron_Idle::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
	
}

void CSlime_Cauldron_Idle::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (m_bAnimationFinished)
	{
		m_pSlime_Cauldron->Change_State(Make_Slime_CauldronState(SLIME_CAULDRON_STATE::ATTACK));
		return;
	}

	_float4 playerPos = m_pSlime_Cauldron->Get_Player_Position(TEXT("GameObject_Player"),
		m_pGameInstance->Get_CurrentLevelIndex());

	m_pTransformCom->LookAt(XMLoadFloat4(&playerPos));
}

void CSlime_Cauldron_Idle::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CSlime_Cauldron_Idle::State_Exit()
{
}

void CSlime_Cauldron_Idle::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);
}

void CSlime_Cauldron_Idle::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CSlime_Cauldron_Idle::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Monster* CSlime_Cauldron_Idle::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATE_SLIME_CAULDRON_DESC* pDesc)
{
	CSlime_Cauldron_Idle* pGameInstance = new CSlime_Cauldron_Idle(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CSlime_Cauldron_Idle");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CSlime_Cauldron_Idle::Free()
{
	__super::Free();
}
