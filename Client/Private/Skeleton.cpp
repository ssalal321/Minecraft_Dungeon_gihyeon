#include "Skeleton.h"
#include "GameInstance.h"

#include "Body_Skeleton.h"

#include "FSM.h"
#include "Skeleton_BowAction.h"
#include "Skeleton_GetHit.h"
#include "Skeleton_HeadSpin.h"
#include "Skeleton_Idle.h"
#include "Skeleton_Walk.h"
#include "State_Monster.h"
//#include "Skeleton_BowAction.h"
//#include "Skeleton_GetHit.h"
//#include "Skeleton_Idle.h"
//#include "Skeleton_Sleep.h"
//#include "Skeleton_Stun.h"
//#include "Skeleton_Walk.h"


CSkeleton::CSkeleton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CSkeleton::CSkeleton(const CSkeleton& Prototype)
	: CMonster(Prototype)
{
}

HRESULT CSkeleton::Initialize_Prototype()
{
	/* 외부 데이터베이스를 통해서 값을 채운다. */

	return S_OK;
}

HRESULT CSkeleton::Initialize(void* pArg)
{
	m_pMonsterInfo = new MONSTER_DESC(TEXT("GameObject_Skeleton"), 10, 10, 2, 10.f, 14.f, false, 90.f, 1.5f);

	if (FAILED(__super::Initialize(m_pMonsterInfo)))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_States()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
								XMVectorSet(-5.f, 0.f, -10.f, 1.f));

	return S_OK;
}

void CSkeleton::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CSkeleton::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CSkeleton::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CSkeleton::Render()
{
	return S_OK;
}


HRESULT CSkeleton::Ready_PartObjects()
{
	/* 몸통을 추가한다. */
	CBody_Skeleton::BODY_SKELETON_DESC		BodyDesc{};

	BodyDesc.strGameObjectTag = TEXT("GameObject_Body_Skeleton");
	BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	BodyDesc.pState = &m_iState;
	BodyDesc.pContainerObject = this;
	BodyDesc.pContainerObjAttacking = &m_bAttacking;

	if (FAILED(__super::Add_PartObject(m_pGameInstance->Get_PrototypeLevelIndex(), TEXT("Prototype_GameObject_Body_Skeleton"), TEXT("Part_Body"), &BodyDesc)))
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

	//if (FAILED(__super::Add_PartObject(m_pGameInstance->Get_PrototypeLevelIndex(), TEXT("Prototype_GameObject_Weapon"), TEXT("Part_Weapon"), &WeaponDesc)))
	//	return E_FAIL;


	/* 이펙트를 추가한다. */

	return S_OK;
}

HRESULT CSkeleton::Ready_States()
{
	m_StatesVec.resize(static_cast<_uint>(SKELETON_STATE::STATE_END));	// state vector 자리 예약

	CModel* pSkeletonModel = dynamic_cast<CModel*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Model")));
	CCollider* pColliderOBB = dynamic_cast<CCollider*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_OBB")));
	CCollider* pColliderSphere = dynamic_cast<CCollider*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_Sphere")));

	CState_Monster::STATEMONSTER_DESC	pStateMonsterDesc = {};
	pStateMonsterDesc.pColliderOBBCom		= pColliderOBB;
	pStateMonsterDesc.pColliderSphereCom	= pColliderSphere;
	pStateMonsterDesc.pActorModelCom		= pSkeletonModel;
	pStateMonsterDesc.pNavigationCom		= m_pNavigationCom;
	pStateMonsterDesc.pTransformCom			= m_pTransformCom;

	m_StatesVec[static_cast<_uint>(SKELETON_STATE::IDLE)]			 = CSkeleton_Idle::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(SKELETON_STATE::WALK)]			 = CSkeleton_Walk::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(SKELETON_STATE::BOW_ACTION)]		 = CSkeleton_BowAction::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(SKELETON_STATE::GET_HIT_FRONT)]	 = CSkeleton_GetHit::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	//m_StatesVec[static_cast<_uint>(SKELETON_STATE::STUN)]			 = CSkeleton_Stun::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(SKELETON_STATE::HEAD_SPIN)] = CSkeleton_HeadSpin::Create(this, m_pMonsterInfo, &pStateMonsterDesc);

	m_pMonsterFSM = FSM::Create();

	m_pMonsterFSM->Init_State(m_StatesVec[static_cast<_uint>(SKELETON_STATE::IDLE)]);

	return S_OK;
}

CSkeleton* CSkeleton::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkeleton* pGameInstance = new CSkeleton(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CSkeleton");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CGameObject* CSkeleton::Clone(void* pArg)
{
	CSkeleton* pGameInstance = new CSkeleton(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CSkeleton");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CSkeleton::Free()
{
	__super::Free();

	for (auto& stateVec : m_StatesVec)
	{
		if (stateVec!= nullptr)
			Safe_Release(stateVec) ;
	}
}
