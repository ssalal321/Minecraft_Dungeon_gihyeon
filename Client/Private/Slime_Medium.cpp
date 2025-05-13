#include "Slime_Medium.h"
#include "GameInstance.h"

#include "Body_Slime_Medium.h"

#include "FSM.h"
#include "Slime_Medium_Attack.h"
#include "Slime_Medium_Idle.h"
#include "Slime_Medium_Stun.h"
#include "Slime_Medium_Walk.h"

_int  CSlime_Medium::m_iSlime_MediumID = 0;

CSlime_Medium::CSlime_Medium(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CSlime_Medium::CSlime_Medium(const CSlime_Medium& Prototype)
	: CMonster(Prototype)
{
}

HRESULT CSlime_Medium::Initialize_Prototype()
{
	/* 외부 데이터베이스를 통해서 값을 채운다. */

	return S_OK;
}

HRESULT CSlime_Medium::Initialize(void* pArg)
{
	const _wstring& Slime_MediumGameObjectTag = TEXT("GameObject_Slime_Medium_") + to_wstring(m_iSlime_MediumID++);

	m_pMonsterInfo = new MONSTER_DESC(Slime_MediumGameObjectTag, 30, 30, 2, 3.f, 5.5f, false, 90.f, 3.f);

	if (FAILED(__super::Initialize(m_pMonsterInfo)))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_States()))
		return E_FAIL;

	SLIME_MEDIUM_DESC* pDesc = static_cast<SLIME_MEDIUM_DESC*>(pArg);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
								XMLoadFloat4(&pDesc->slimeMediumPosition));

	return S_OK;
}

void CSlime_Medium::Priority_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Priority_Update(fTimeDelta);
}

void CSlime_Medium::Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Update(fTimeDelta);
}

void CSlime_Medium::Late_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Update(fTimeDelta);
}

HRESULT CSlime_Medium::Render()
{
	if (!m_bActive)
		return S_OK;

	return S_OK;
}


HRESULT CSlime_Medium::Ready_PartObjects()
{
	/* 몸통을 추가한다. */
	CBody_Slime_Medium::BODY_SLIME_MEDIUM_DESC		BodyDesc{};

	BodyDesc.strGameObjectTag			= TEXT("GameObject_Body_Slime_Medium");
	BodyDesc.pParentWorldMatrix			= m_pTransformCom->Get_WorldMatrix_Ptr();
	BodyDesc.pState						= &m_iState;
	BodyDesc.pContainerObject			= this;
	BodyDesc.pBigCollisionActivating	= &m_bAttacking;
	BodyDesc.pSmallCollisionActivating	= &m_bAlwaysActivated;

	if (FAILED(__super::Add_PartObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Body_Slime_Medium"), TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;

	
	/* 이펙트를 추가한다. */

	return S_OK;
}

HRESULT CSlime_Medium::Ready_States()
{
	m_StatesVec.resize(static_cast<_uint>(SLIME_MEDIUM_STATE::STATE_END));	// state vector 자리 예약

	CModel* pSlime_MediumModel = dynamic_cast<CModel*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Model")));
	CCollider* pBigCollider = dynamic_cast<CCollider*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
	CCollider* pSmallCollider = dynamic_cast<CCollider*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_SmallSphere")));\

	CState_Monster::STATEMONSTER_DESC	pStateMonsterDesc = {};
	pStateMonsterDesc.pBigColliderCom	= pBigCollider;
	pStateMonsterDesc.pSmallColliderCom = pSmallCollider;
	pStateMonsterDesc.pActorModelCom	= pSlime_MediumModel;
	pStateMonsterDesc.pNavigationCom	= m_pNavigationCom;
	pStateMonsterDesc.pTransformCom		= m_pTransformCom;

	m_StatesVec[static_cast<_uint>(SLIME_MEDIUM_STATE::IDLE)]	= CSlime_Medium_Idle::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(SLIME_MEDIUM_STATE::WALK)]	= CSlime_Medium_Walk::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(SLIME_MEDIUM_STATE::ATTACK)]	= CSlime_Medium_Attack::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(SLIME_MEDIUM_STATE::STUN)]	= CSlime_Medium_Stun::Create(this, m_pMonsterInfo, &pStateMonsterDesc);

	m_pMonsterFSM = FSM::Create();

	m_pMonsterFSM->Init_State(m_StatesVec[static_cast<_uint>(SLIME_MEDIUM_STATE::IDLE)]);

	return S_OK;
}

CSlime_Medium* CSlime_Medium::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSlime_Medium* pGameInstance = new CSlime_Medium(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CSlime_Medium");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CGameObject* CSlime_Medium::Clone(void* pArg)
{
	CSlime_Medium* pGameInstance = new CSlime_Medium(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CSlime_Medium");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CSlime_Medium::Free()
{
	__super::Free();

	for (auto& stateVec : m_StatesVec)
	{
		if (stateVec!= nullptr)
			Safe_Release(stateVec) ;
	}
}
