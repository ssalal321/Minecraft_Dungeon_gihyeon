#include "Slime_Small.h"
#include "GameInstance.h"

#include "Body_Slime_Small.h"

#include "FSM.h"
#include "Slime_Small_Attack.h"
#include "Slime_Small_Idle.h"
#include "Slime_Small_Novelty.h"
#include "Slime_Small_Walk.h"

_int  CSlime_Small::m_iSlime_SmallID = 0;

CSlime_Small::CSlime_Small(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CSlime_Small::CSlime_Small(const CSlime_Small& Prototype)
	: CMonster(Prototype)
{
}

HRESULT CSlime_Small::Initialize_Prototype()
{
	/* 외부 데이터베이스를 통해서 값을 채운다. */

	return S_OK;
}

HRESULT CSlime_Small::Initialize(void* pArg)
{
	const _wstring& Slime_SmallGameObjectTag = TEXT("GameObject_Slime_Small_") + to_wstring(m_iSlime_SmallID++);

	m_pMonsterInfo = new MONSTER_DESC(Slime_SmallGameObjectTag, 60, 60, 10, 3.f, 5.5f, false, 90.f, 3.f);

	if (FAILED(__super::Initialize(m_pMonsterInfo)))
		return E_FAIL;

	if (m_pNavigationCom)
		m_pNavigationCom->SetUp_CurrentCellIndex(0);

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_States()))
		return E_FAIL;

	SLIME_SMALL_DESC* pDesc = static_cast<SLIME_SMALL_DESC*>(pArg);

	//m_pTransformCom->SetUp_Scale(0.6f, 0.6f, 0.6f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
								XMLoadFloat4(&pDesc->slimeSmallPosition));

	return S_OK;
}

void CSlime_Small::Priority_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Priority_Update(fTimeDelta);
}

void CSlime_Small::Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Update(fTimeDelta);
}

void CSlime_Small::Late_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Update(fTimeDelta);
}

HRESULT CSlime_Small::Render()
{
	if (!m_bActive)
		return S_OK;

	__super::Render();

	return S_OK;
}


HRESULT CSlime_Small::Ready_PartObjects()
{
	/* 몸통을 추가한다. */
	CBody_Slime_Small::BODY_SLIME_SMALL_DESC		BodyDesc{};

	BodyDesc.strGameObjectTag			= TEXT("GameObject_Body_Slime_Small");
	BodyDesc.pParentWorldMatrix			= m_pTransformCom->Get_WorldMatrix_Ptr();
	BodyDesc.pState						= &m_iState;
	BodyDesc.pContainerObject			= this;
	BodyDesc.pBigCollisionActivating	= &m_bAttacking;
	BodyDesc.pSmallCollisionActivating	= &m_bAlwaysActivated;

	if (FAILED(__super::Add_PartObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Body_Slime_Small"), TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;

	
	/* 이펙트를 추가한다. */

	return S_OK;
}

HRESULT CSlime_Small::Ready_States()
{
	m_StatesVec.resize(static_cast<_uint>(SLIME_SMALL_STATE::STATE_END));	// state vector 자리 예약

	CModel* pSlime_SmallModel = dynamic_cast<CModel*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Model")));
	CCollider* pBigCollider = dynamic_cast<CCollider*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
	CCollider* pSmallCollider = dynamic_cast<CCollider*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_SmallSphere")));\

	CState_Monster::STATEMONSTER_DESC	pStateMonsterDesc = {};
	pStateMonsterDesc.pBigColliderCom	= pBigCollider;
	pStateMonsterDesc.pSmallColliderCom = pSmallCollider;
	pStateMonsterDesc.pActorModelCom	= pSlime_SmallModel;
	pStateMonsterDesc.pNavigationCom	= m_pNavigationCom;
	pStateMonsterDesc.pTransformCom		= m_pTransformCom;

	m_StatesVec[static_cast<_uint>(SLIME_SMALL_STATE::IDLE)]	= CSlime_Small_Idle::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(SLIME_SMALL_STATE::WALK)]	= CSlime_Small_Walk::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(SLIME_SMALL_STATE::ATTACK)]	= CSlime_Small_Attack::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(SLIME_SMALL_STATE::NOVELTY)]	= CSlime_Small_Novelty::Create(this, m_pMonsterInfo, &pStateMonsterDesc);

	m_pMonsterFSM = FSM::Create();

	m_pMonsterFSM->Init_State(m_StatesVec[static_cast<_uint>(SLIME_SMALL_STATE::IDLE)]);

	return S_OK;
}

CSlime_Small* CSlime_Small::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSlime_Small* pGameInstance = new CSlime_Small(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CSlime_Small");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CGameObject* CSlime_Small::Clone(void* pArg)
{
	CSlime_Small* pGameInstance = new CSlime_Small(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CSlime_Small");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CSlime_Small::Free()
{
	__super::Free();

	for (auto& stateVec : m_StatesVec)
	{
		if (stateVec!= nullptr)
			Safe_Release(stateVec) ;
	}
}
