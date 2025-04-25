#include "Zombie.h"
#include "GameInstance.h"

#include "Body_Zombie.h"

#include "FSM.h"
#include "Zombie_Attack.h"
#include "Zombie_GetHit.h"
#include "Zombie_Idle.h"
#include "Zombie_Sleep.h"
#include "Zombie_Stun.h"
#include "Zombie_Walk.h"

CZombie::CZombie(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CZombie::CZombie(const CZombie& Prototype)
	: CMonster(Prototype)
{
}

HRESULT CZombie::Initialize_Prototype()
{
	/* 외부 데이터베이스를 통해서 값을 채운다. */

	return S_OK;
}

HRESULT CZombie::Initialize(void* pArg)
{
	m_pMonsterInfo = new MONSTER_DESC(TEXT("GameObject_Zombie"), 10, 10, 2, 1.8f, 10.f, false, 90.f, 1.5f);

	if (FAILED(__super::Initialize(m_pMonsterInfo)))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_States()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
								XMVectorSet(0.f, 0.f, -10.f, 1.f));

	return S_OK;
}

void CZombie::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CZombie::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CZombie::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CZombie::Render()
{
	return S_OK;
}


HRESULT CZombie::Ready_PartObjects()
{
	/* 몸통을 추가한다. */
	CBody_Zombie::BODY_ZOMBIE_DESC		BodyDesc{};

	BodyDesc.strGameObjectTag = TEXT("GameObject_Body_Zombie");
	BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	BodyDesc.pState = &m_iState;
	BodyDesc.pContainerObject = this;
	BodyDesc.pContainerObjAttacking = &m_bAttacking;

	if (FAILED(__super::Add_PartObject(m_pGameInstance->Get_PrototypeLevelIndex(), TEXT("Prototype_GameObject_Body_Zombie"), TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;


	///* 무기를 추가한다. */
	//CWeapon::WEAPON_DESC	WeaponDesc{};

	//CModel* pBody = dynamic_cast<CModel*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Model")));
	//if (nullptr == pBody)
	//	return E_FAIL;

	//WeaponDesc.pGameObjectTag = TEXT("GameObject_Weapon");
	//WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	//WeaponDesc.pState = &m_iState;
	//WeaponDesc.pSocketMatrix = pBody->Get_CombinedTransformationMatrix("J_R_Weapon");

	//if (FAILED(__super::Add_PartObject(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_GameObject_Weapon"), TEXT("Part_Weapon"), &WeaponDesc)))
	//	return E_FAIL;


	/* 이펙트를 추가한다. */

	return S_OK;
}

HRESULT CZombie::Ready_States()
{
	m_StatesVec.resize(static_cast<_uint>(ZOMBIE_STATE::STATE_END));	// state vector 자리 예약

	CModel* pZombieModel = dynamic_cast<CModel*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Model")));
	CCollider* pColliderOBB = dynamic_cast<CCollider*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_OBB")));
	CCollider* pColliderSphere = dynamic_cast<CCollider*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_Sphere")));

	CState_Monster::STATEMONSTER_DESC	pStateMonsterDesc = {};
	pStateMonsterDesc.pColliderOBBCom		= pColliderOBB;
	pStateMonsterDesc.pColliderSphereCom	= pColliderSphere;
	pStateMonsterDesc.pActorModelCom		= pZombieModel;
	pStateMonsterDesc.pNavigationCom		= m_pNavigationCom;
	pStateMonsterDesc.pTransformCom			= m_pTransformCom;

	m_StatesVec[static_cast<_uint>(ZOMBIE_STATE::IDLE)]			 = CZombie_Idle::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(ZOMBIE_STATE::WALK)]			 = CZombie_Walk::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(ZOMBIE_STATE::ATTACK)]		 = CZombie_Attack::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(ZOMBIE_STATE::GET_HIT_FRONT)] = CZombie_GetHit::Create(this, m_pMonsterInfo, &pStateMonsterDesc);  // Get_Hit_Left/Right도 포함
	m_StatesVec[static_cast<_uint>(ZOMBIE_STATE::STUN)]			 = CZombie_Stun::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(ZOMBIE_STATE::NOVELTY_SLEEP)] = CZombie_Sleep::Create(this, m_pMonsterInfo, &pStateMonsterDesc);

	m_pMonsterFSM = FSM::Create();

	m_pMonsterFSM->Init_State(m_StatesVec[static_cast<_uint>(ZOMBIE_STATE::IDLE)]);

	return S_OK;
}

CZombie* CZombie::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CZombie* pGameInstance = new CZombie(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CZombie");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CGameObject* CZombie::Clone(void* pArg)
{
	CZombie* pGameInstance = new CZombie(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CZombie");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CZombie::Free()
{
	__super::Free();

	for (auto& stateVec : m_StatesVec)
	{
		if (stateVec!= nullptr)
			Safe_Release(stateVec) ;
	}
}
