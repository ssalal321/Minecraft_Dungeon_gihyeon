#include "Slime_Large.h"
#include "GameInstance.h"

#include "Body_Slime_Large.h"

#include "FSM.h"
#include "Slime_Large_Attack.h"
#include "Slime_Large_Idle.h"
#include "Slime_Large_Stun.h"
#include "Slime_Large_Walk.h"

_int  CSlime_Large::m_iSlime_LargeID = 0;

CSlime_Large::CSlime_Large(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CSlime_Large::CSlime_Large(const CSlime_Large& Prototype)
	: CMonster(Prototype)
{
}

HRESULT CSlime_Large::Initialize_Prototype()
{
	/* 외부 데이터베이스를 통해서 값을 채운다. */

	return S_OK;
}

HRESULT CSlime_Large::Initialize(void* pArg)
{
	const _wstring& Slime_LargeGameObjectTag = TEXT("GameObject_Slime_Large_") + to_wstring(m_iSlime_LargeID++);

	m_pMonsterInfo = new MONSTER_DESC(Slime_LargeGameObjectTag, 20, 20, 2, 3.f, 10.f, false, 90.f, 0.7f);

	if (FAILED(__super::Initialize(m_pMonsterInfo)))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_States()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
								XMVectorSet(3.f, 0.f, -7.f, 1.f));

	return S_OK;
}

void CSlime_Large::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CSlime_Large::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CSlime_Large::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CSlime_Large::Render()
{
	return S_OK;
}


HRESULT CSlime_Large::Ready_PartObjects()
{
	/* 몸통을 추가한다. */
	CBody_Slime_Large::BODY_SLIME_LARGE_DESC		BodyDesc{};

	BodyDesc.strGameObjectTag = TEXT("GameObject_Body_Slime_Large");
	BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	BodyDesc.pState = &m_iState;
	BodyDesc.pContainerObject = this;
	BodyDesc.pCollisionActivating = &m_bAttacking;

	if (FAILED(__super::Add_PartObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Body_Slime_Large"), TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;

	
	/* 이펙트를 추가한다. */

	return S_OK;
}

HRESULT CSlime_Large::Ready_States()
{
	m_StatesVec.resize(static_cast<_uint>(SLIME_LARGE_STATE::STATE_END));	// state vector 자리 예약

	CModel* pSlime_LargeModel = dynamic_cast<CModel*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Model")));
	CCollider* pCollider = dynamic_cast<CCollider*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_Sphere")));

	CState_Monster::STATEMONSTER_DESC	pStateMonsterDesc = {};
	pStateMonsterDesc.pColliderCom		= pCollider;
	pStateMonsterDesc.pActorModelCom	= pSlime_LargeModel;
	pStateMonsterDesc.pNavigationCom	= m_pNavigationCom;
	pStateMonsterDesc.pTransformCom		= m_pTransformCom;

	m_StatesVec[static_cast<_uint>(SLIME_LARGE_STATE::IDLE)]	= CSlime_Large_Idle::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(SLIME_LARGE_STATE::WALK)]	= CSlime_Large_Walk::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(SLIME_LARGE_STATE::ATTACK)]	= CSlime_Large_Attack::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(SLIME_LARGE_STATE::STUN)]	= CSlime_Large_Stun::Create(this, m_pMonsterInfo, &pStateMonsterDesc);

	m_pMonsterFSM = FSM::Create();

	m_pMonsterFSM->Init_State(m_StatesVec[static_cast<_uint>(SLIME_LARGE_STATE::IDLE)]);

	return S_OK;
}

CSlime_Large* CSlime_Large::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSlime_Large* pGameInstance = new CSlime_Large(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CSlime_Large");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CGameObject* CSlime_Large::Clone(void* pArg)
{
	CSlime_Large* pGameInstance = new CSlime_Large(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CSlime_Large");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CSlime_Large::Free()
{
	__super::Free();

	for (auto& stateVec : m_StatesVec)
	{
		if (stateVec!= nullptr)
			Safe_Release(stateVec) ;
	}
}
