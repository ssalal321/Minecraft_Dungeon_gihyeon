#include "Slime_Cauldron.h"

#include "Body_Slime_Cauldron.h"
#include "BulletPool_Monster.h"
#include "GameInstance.h"

#include "FSM.h"
#include "Slime_Cauldron_Attack.h"
#include "Slime_Cauldron_Idle.h"

_int  CSlime_Cauldron::m_iSlimeCauldronID = 0;

CSlime_Cauldron::CSlime_Cauldron(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CSlime_Cauldron::CSlime_Cauldron(const CSlime_Cauldron& Prototype)
	: CMonster(Prototype)
{
}

HRESULT CSlime_Cauldron::Initialize_Prototype()
{
	/* 외부 데이터베이스를 통해서 값을 채운다. */

	return S_OK;
}

HRESULT CSlime_Cauldron::Initialize(void* pArg)
{
	const _wstring& SlimeCauldronGameObjectTag = TEXT("GameObject_Slime_Cauldron_") + to_wstring(m_iSlimeCauldronID++);

	m_pMonsterInfo = new MONSTER_DESC(SlimeCauldronGameObjectTag, 30, 30, 2, 3.f, 5.5f, false, 90.f, 3.f);

	if (FAILED(__super::Initialize(m_pMonsterInfo)))
		return E_FAIL;

	if (m_pNavigationCom)
		m_pNavigationCom->SetUp_CurrentCellIndex(0);

	m_pBulletPool_Monster = CBulletPool_Monster::Create();
	if (nullptr == m_pBulletPool_Monster)
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_States()))
		return E_FAIL;

	m_pTransformCom->SetUp_Scale(0.85f, 0.85f, 0.85f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(-0.f, 0.f, -15.f, 1.f));

	/*SLIME_CORRUPTED_DESC* pDesc = static_cast<SLIME_CORRUPTED_DESC*>(pArg);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pDesc->slimeCauldronPosition));*/

	return S_OK;
}

void CSlime_Cauldron::Priority_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Priority_Update(fTimeDelta);
}

void CSlime_Cauldron::Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Update(fTimeDelta);
}

void CSlime_Cauldron::Late_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Update(fTimeDelta);
}

HRESULT CSlime_Cauldron::Render()
{
	if (!m_bActive)
		return S_OK;

	__super::Render();

	return S_OK;
}


HRESULT CSlime_Cauldron::Ready_PartObjects()
{
	/* 몸통을 추가한다. */
	CBody_Slime_Cauldron::BODY_SLIME_CAULDRON_DESC		BodyDesc{};

	BodyDesc.strGameObjectTag			= TEXT("GameObject_Body_Slime_Cauldron");
	BodyDesc.pParentWorldMatrix			= m_pTransformCom->Get_WorldMatrix_Ptr();
	BodyDesc.pState						= &m_iState;
	BodyDesc.pContainerObject			= this;
	BodyDesc.pBigCollisionActivating	= &m_bAttacking;
	BodyDesc.pSmallCollisionActivating	= &m_bAlwaysActivated;

	if (FAILED(__super::Add_PartObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Body_Slime_Cauldron"), TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;

	
	/* 이펙트를 추가한다. */

	return S_OK;
}

HRESULT CSlime_Cauldron::Ready_States()
{
	m_StatesVec.resize(static_cast<_uint>(SLIME_CAULDRON_STATE::STATE_END));	// state vector 자리 예약

	CModel* pSlimeCorruptedModel = dynamic_cast<CModel*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Model")));
	CCollider* pBigCollider = dynamic_cast<CCollider*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
	CCollider* pSmallCollider = dynamic_cast<CCollider*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_SmallSphere")));\

	CState_Slime_Cauldron::STATE_SLIME_CAULDRON_DESC	pStateSlimeCauldronDesc = {};
	pStateSlimeCauldronDesc.pBigColliderCom		= pBigCollider;
	pStateSlimeCauldronDesc.pSmallColliderCom	= pSmallCollider;
	pStateSlimeCauldronDesc.pActorModelCom		= pSlimeCorruptedModel;
	pStateSlimeCauldronDesc.pNavigationCom		= m_pNavigationCom;
	pStateSlimeCauldronDesc.pTransformCom		= m_pTransformCom;
	pStateSlimeCauldronDesc.pBulletPool_Monster = m_pBulletPool_Monster;

	m_StatesVec[static_cast<_uint>(SLIME_CAULDRON_STATE::ATTACK)] = CSlime_Cauldron_Attack::Create(this, m_pMonsterInfo, &pStateSlimeCauldronDesc);
	m_StatesVec[static_cast<_uint>(SLIME_CAULDRON_STATE::IDLE)]   = CSlime_Cauldron_Idle::Create(this, m_pMonsterInfo, &pStateSlimeCauldronDesc);

	m_pMonsterFSM = FSM::Create();

	m_pMonsterFSM->Init_State(m_StatesVec[static_cast<_uint>(SLIME_CAULDRON_STATE::IDLE)]);

	return S_OK;
}

CSlime_Cauldron* CSlime_Cauldron::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSlime_Cauldron* pGameInstance = new CSlime_Cauldron(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CSlime_Cauldron");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CGameObject* CSlime_Cauldron::Clone(void* pArg)
{
	CSlime_Cauldron* pGameInstance = new CSlime_Cauldron(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CSlime_Cauldron");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CSlime_Cauldron::Free()
{
	__super::Free();

	Safe_Release(m_pBulletPool_Monster);

	for (auto& stateVec : m_StatesVec)
	{
		if (stateVec!= nullptr)
			Safe_Release(stateVec) ;
	}
}
