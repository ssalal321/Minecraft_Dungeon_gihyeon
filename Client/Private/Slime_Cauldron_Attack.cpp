#include "Slime_Cauldron_Attack.h"
#include "State_Slime_Cauldron.h"
#include "Slime_Cauldron.h"

#include <random>

#include "BulletPool_Monster.h"
#include "Player.h"
#include "Slime_Cauldron_Bullet.h"

#define FIRE_BULLET  5.f

CSlime_Cauldron_Attack::CSlime_Cauldron_Attack(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATE_SLIME_CAULDRON_DESC* pDesc)
	: Client::CState_Slime_Cauldron(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CSlime_Cauldron_Attack::Init_State()
{
	__super::Init_State();

	m_pSlime_Cauldron = dynamic_cast<CSlime_Cauldron*>(m_pActor);
	if (nullptr == m_pSlime_Cauldron)
		return E_FAIL;

	m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject(TEXT("GameObject_Player"), m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Player")));
	if (!m_pPlayer)
		return E_FAIL;

	return S_OK;
}

void CSlime_Cauldron_Attack::State_Enter()
{
	m_bShot = false;

	m_pActorModelCom->Set_Animation(static_cast<_uint>(SLIME_CAULDRON_STATE::ATTACK), false, 1.f);
}

void CSlime_Cauldron_Attack::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
	
}

void CSlime_Cauldron_Attack::State_Update(_float fTimeDelta)
{
	__super::State_Update(fTimeDelta);

	_float fAnimCurTrackPos = m_pActorModelCom->Get_AnimCurrentTrackPosition();

	if (m_bAnimationFinished)
	{
		m_pSlime_Cauldron->Change_State(Make_Slime_CauldronState(SLIME_CAULDRON_STATE::IDLE));
		return;
	}

	if (!m_bShot && FIRE_BULLET <= fAnimCurTrackPos)
		Fire_Bullet();

	if (m_bShot)
		return;

	m_PlayerPosition = m_pSlime_Cauldron->Get_Player_Position(TEXT("GameObject_Player"),
															  m_pGameInstance->Get_CurrentLevelIndex());

	m_pTransformCom->LookAt(XMLoadFloat4(&m_PlayerPosition));
}


void CSlime_Cauldron_Attack::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CSlime_Cauldron_Attack::State_Exit()
{
}

void CSlime_Cauldron_Attack::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);

	Modify_HP(pOther);
}

void CSlime_Cauldron_Attack::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CSlime_Cauldron_Attack::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

void CSlime_Cauldron_Attack::Fire_Bullet()
{
	CSlime_Cauldron_Bullet* pBullet = m_pBulletPool_Monster->Get_Bullet(m_pMonsterInfo->Get_DealPoint());
	if (nullptr == pBullet)
		return;

	m_bShot = true;

	_float4	 monsterPos = {};
	XMStoreFloat4(&monsterPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	pBullet->Fire(monsterPos, m_PlayerPosition);
}

CState_Monster* CSlime_Cauldron_Attack::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATE_SLIME_CAULDRON_DESC* pDesc)
{
	CSlime_Cauldron_Attack* pGameInstance = new CSlime_Cauldron_Attack(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CSlime_Cauldron_Attack");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CSlime_Cauldron_Attack::Free()
{
	__super::Free();
}
