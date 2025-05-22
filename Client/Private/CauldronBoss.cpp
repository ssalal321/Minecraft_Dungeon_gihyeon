#include "CauldronBoss.h"
#include "GameInstance.h"

#include "Body_CauldronBoss.h"
#include "Boss_Trigger.h"

#include "FSM.h"
#include "CauldronBoss_BasicAttack.h"
#include "CauldronBoss_Idle.h"
#include "CauldronBoss_IntenseSpin.h"
#include "CauldronBoss_StrongAttack.h"
#include "CauldronBoss_TPose.h"

_int  CCauldronBoss::m_iCauldronBossID = 0;

CCauldronBoss::CCauldronBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CCauldronBoss::CCauldronBoss(const CCauldronBoss& Prototype)
	: CMonster(Prototype)
{
}

HRESULT CCauldronBoss::Initialize_Prototype()
{
	/* 외부 데이터베이스를 통해서 값을 채운다. */

	return S_OK;
}

HRESULT CCauldronBoss::Initialize(void* pArg)
{
	const _wstring& CauldronBossGameObjectTag = TEXT("GameObject_CauldronBoss_") + to_wstring(m_iCauldronBossID++);

	m_pMonsterInfo = new MONSTER_DESC(CauldronBossGameObjectTag, 2000, 2000, 75, 3.f, 5.5f, false, 90.f, 3.f);

	if (FAILED(__super::Initialize(m_pMonsterInfo)))
		return E_FAIL;

	/*if (m_pNavigationCom)
		m_pNavigationCom->SetUp_CurrentCellIndex(0);*/

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_States()))
		return E_FAIL;

	CAULDRONBOSS_DESC* pDesc = static_cast<CAULDRONBOSS_DESC*>(pArg);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMLoadFloat4(&pDesc->slimeCauldronPosition));

	m_pTransformCom->SetUp_Scale(0.7f, 0.7f, 0.7f);

	if (m_pNavigationCom)
		m_pNavigationCom->SetUp_CurrentCellIndex(869);

	m_bStationary = true;
	m_bActive = false;

	return S_OK;
}

void CCauldronBoss::Priority_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Priority_Update(fTimeDelta);
}

void CCauldronBoss::Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	

	__super::Update(fTimeDelta);
}

void CCauldronBoss::Late_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Update(fTimeDelta);
}

HRESULT CCauldronBoss::Render()
{
	if (!m_bActive)
		return S_OK;

	__super::Render();

	return S_OK;
}


HRESULT CCauldronBoss::Ready_PartObjects()
{
	/* 몸통을 추가한다. */
	CBody_CauldronBoss::BODY_CAULDRONBOSS_DESC		BodyDesc{};

	BodyDesc.strGameObjectTag			= TEXT("GameObject_Body_CauldronBoss");
	BodyDesc.pParentWorldMatrix			= m_pTransformCom->Get_WorldMatrix_Ptr();
	BodyDesc.pState						= &m_iState;
	BodyDesc.pContainerObject			= this;
	BodyDesc.pBigCollisionActivating	= &m_bAttacking;
	BodyDesc.pSmallCollisionActivating	= &m_bAlwaysActivated;

	if (FAILED(__super::Add_PartObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Body_CauldronBoss"), TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;

	/* 이펙트를 추가한다. */

	return S_OK;
}

HRESULT CCauldronBoss::Ready_States()
{
	m_StatesVec.resize(static_cast<_uint>(CAULDRONBOSS_STATE::STATE_END));	// state vector 자리 예약

	CModel* pCauldronBossModel = dynamic_cast<CModel*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Model")));
	CCollider* pBigCollider = dynamic_cast<CCollider*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
	CCollider* pSmallCollider = dynamic_cast<CCollider*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_SmallSphere")));\

	CState_Monster::STATEMONSTER_DESC	pStateMonsterDesc = {};
	pStateMonsterDesc.pBigColliderCom	= pBigCollider;
	pStateMonsterDesc.pSmallColliderCom = pSmallCollider;
	pStateMonsterDesc.pActorModelCom	= pCauldronBossModel;
	pStateMonsterDesc.pNavigationCom	= m_pNavigationCom;
	pStateMonsterDesc.pTransformCom		= m_pTransformCom;

	m_StatesVec[static_cast<_uint>(CAULDRONBOSS_STATE::IDLE)]			= CCauldronBoss_Idle::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(CAULDRONBOSS_STATE::STRONG_ATTACK)]	= CCauldronBoss_StrongAttack::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(CAULDRONBOSS_STATE::BASIC_ATTACK)]	= CCauldronBoss_BasicAttack::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(CAULDRONBOSS_STATE::INTENSE_SPIN)]	= CCauldronBoss_IntenseSpin::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(CAULDRONBOSS_STATE::TPOSE)]			= CCauldronBoss_TPose::Create(this, m_pMonsterInfo, &pStateMonsterDesc);

	m_pMonsterFSM = FSM::Create();

	m_pMonsterFSM->Init_State(m_StatesVec[static_cast<_uint>(CAULDRONBOSS_STATE::TPOSE)]);

	return S_OK;
}

CCauldronBoss* CCauldronBoss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCauldronBoss* pGameInstance = new CCauldronBoss(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CCauldronBoss");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CGameObject* CCauldronBoss::Clone(void* pArg)
{
	CCauldronBoss* pGameInstance = new CCauldronBoss(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CCauldronBoss");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CCauldronBoss::Free()
{
	__super::Free();

	for (auto& stateVec : m_StatesVec)
	{
		if (stateVec!= nullptr)
			Safe_Release(stateVec) ;
	}
}
