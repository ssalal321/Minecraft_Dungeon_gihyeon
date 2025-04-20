#include "Zombie_Sleep.h"
#include "Zombie.h"

CZombie_Sleep::CZombie_Sleep(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_Zombie(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CZombie_Sleep::Init_State()
{
	__super::Init_State();

	m_pZombie = dynamic_cast<CZombie*>(m_pActor);
	if (nullptr == m_pZombie)
		return E_FAIL;

	return S_OK;
}

void CZombie_Sleep::State_Enter()
{
	m_pActorModelCom->Set_Animation(static_cast<_uint>(ZOMBIE_STATE::NOVELTY_SLEEP), false);
}

void CZombie_Sleep::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CZombie_Sleep::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	if (Change_State_To_Attack())
		return;

	if (Change_State_To_Walk())
		return;

	if (m_bAnimationFinished)
	{
		m_pZombie->Change_State(ZOMBIE_STATE::IDLE);
	}
}

void CZombie_Sleep::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CZombie_Sleep::State_Exit()
{
}

void CZombie_Sleep::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);
}

void CZombie_Sleep::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CZombie_Sleep::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Monster* CZombie_Sleep::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
{
	CZombie_Sleep* pGameInstance = new CZombie_Sleep(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CZombie_Sleep");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CZombie_Sleep::Free()
{
	__super::Free();
}
