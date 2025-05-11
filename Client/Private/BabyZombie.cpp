#include "BabyZombie.h"
#include "GameInstance.h"

#include "Body_BabyZombie.h"

#include "FSM.h"
#include "BabyZombie_Attack.h"
#include "BabyZombie_GetHit.h"
#include "BabyZombie_Idle.h"
#include "BabyZombie_Novelty.h"
#include "BabyZombie_Walk.h"

_int  CBabyZombie::m_iBabyZombieID = 0;

CBabyZombie::CBabyZombie(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CBabyZombie::CBabyZombie(const CBabyZombie& Prototype)
	: CMonster(Prototype)
{
}

HRESULT CBabyZombie::Initialize_Prototype()
{
	/* 외부 데이터베이스를 통해서 값을 채운다. */

	return S_OK;
}

HRESULT CBabyZombie::Initialize(void* pArg)
{
	const _wstring& zombieGameObjectTag = TEXT("GameObject_BabyZombie_") + to_wstring(m_iBabyZombieID++);

	m_pMonsterInfo = new MONSTER_DESC(zombieGameObjectTag, 20, 20, 5, 2.5f, 12.f, false, 90.f, 3.5f);

	if (FAILED(__super::Initialize(m_pMonsterInfo)))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_States()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
								XMVectorSet(-3.f, 0.f, -15.f, 1.f));

	return S_OK;
}

void CBabyZombie::Priority_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Priority_Update(fTimeDelta);
}

void CBabyZombie::Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Update(fTimeDelta);
}

void CBabyZombie::Late_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Update(fTimeDelta);
}

HRESULT CBabyZombie::Render()
{
	if (!m_bActive)
		return S_OK;

	return S_OK;
}


HRESULT CBabyZombie::Ready_PartObjects()
{
	/* 몸통을 추가한다. */
	CBody_BabyZombie::BODY_BABYZOMBIE_DESC		BodyDesc{};

	BodyDesc.strGameObjectTag = TEXT("GameObject_Body_BabyZombie");
	BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	BodyDesc.pState = &m_iState;
	BodyDesc.pContainerObject = this;
	BodyDesc.pCollisionActivating = &m_bAttacking;

	if (FAILED(__super::Add_PartObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Body_BabyZombie"), TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;

	
	/* 이펙트를 추가한다. */

	return S_OK;
}

HRESULT CBabyZombie::Ready_States()
{
	m_StatesVec.resize(static_cast<_uint>(BABYZOMBIE_STATE::STATE_END));	// state vector 자리 예약

	CModel* pBabyZombieModel = dynamic_cast<CModel*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Model")));
	CCollider* pCollider = dynamic_cast<CCollider*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_Sphere")));

	CState_Monster::STATEMONSTER_DESC	pStateMonsterDesc = {};
	pStateMonsterDesc.pColliderCom		= pCollider;
	pStateMonsterDesc.pActorModelCom	= pBabyZombieModel;
	pStateMonsterDesc.pNavigationCom	= m_pNavigationCom;
	pStateMonsterDesc.pTransformCom		= m_pTransformCom;

	m_StatesVec[static_cast<_uint>(BABYZOMBIE_STATE::IDLE)]		= CBabyZombie_Idle::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(BABYZOMBIE_STATE::WALK)]		= CBabyZombie_Walk::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(BABYZOMBIE_STATE::ATTACK)]	= CBabyZombie_Attack::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(BABYZOMBIE_STATE::GET_HIT)]	= CBabyZombie_GetHit::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(BABYZOMBIE_STATE::NOVELTY)]	= CBabyZombie_Novelty::Create(this, m_pMonsterInfo, &pStateMonsterDesc);

	m_pMonsterFSM = FSM::Create();

	m_pMonsterFSM->Init_State(m_StatesVec[static_cast<_uint>(BABYZOMBIE_STATE::IDLE)]);

	return S_OK;
}

CBabyZombie* CBabyZombie::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBabyZombie* pGameInstance = new CBabyZombie(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CBabyZombie");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CGameObject* CBabyZombie::Clone(void* pArg)
{
	CBabyZombie* pGameInstance = new CBabyZombie(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CBabyZombie");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CBabyZombie::Free()
{
	__super::Free();

	for (auto& stateVec : m_StatesVec)
	{
		if (stateVec!= nullptr)
			Safe_Release(stateVec) ;
	}
}
