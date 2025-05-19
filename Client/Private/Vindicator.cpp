#include "Vindicator.h"
#include "GameInstance.h"

#include "Body_Vindicator.h"

#include "FSM.h"
#include "Item.h"
#include "Vindicator_Attack.h"
#include "Vindicator_Idle.h"
#include "Vindicator_Novelty.h"
#include "Vindicator_Walk.h"

_int  CVindicator::m_iVindicatorID = 0;

CVindicator::CVindicator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CVindicator::CVindicator(const CVindicator& Prototype)
	: CMonster(Prototype)
{
}

HRESULT CVindicator::Initialize_Prototype()
{
	/* 외부 데이터베이스를 통해서 값을 채운다. */

	return S_OK;
}

HRESULT CVindicator::Initialize(void* pArg)
{
	const _wstring& zombieGameObjectTag = TEXT("GameObject_Vindicator_") + to_wstring(m_iVindicatorID++);

	m_pMonsterInfo = new MONSTER_DESC(zombieGameObjectTag, 100, 100, 10, 3.f, 12.f, false, 90.f, 3.5f);

	if (FAILED(__super::Initialize(m_pMonsterInfo)))
		return E_FAIL;

	if (m_pNavigationCom)
		m_pNavigationCom->SetUp_CurrentCellIndex(363);

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_States()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
								XMVectorSet(-6.f, 0.05f, -6.5f, 1.f));


	/*VINDICATOR_DESC* pDesc = static_cast<VINDICATOR_DESC*>(pArg);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pDesc->babyZombiePosition));*/

	return S_OK;
}

void CVindicator::Priority_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Priority_Update(fTimeDelta);
}

void CVindicator::Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Update(fTimeDelta);
}

void CVindicator::Late_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Update(fTimeDelta);
}

HRESULT CVindicator::Render()
{
	if (!m_bActive)
		return S_OK;

	__super::Render();

	return S_OK;
}


HRESULT CVindicator::Ready_PartObjects()
{
	/* 몸통을 추가한다. */
	CBody_Vindicator::BODY_VINDICATOR_DESC		BodyDesc{};

	BodyDesc.strGameObjectTag = TEXT("GameObject_Body_Vindicator");
	BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	BodyDesc.pState = &m_iState;
	BodyDesc.pContainerObject = this;
	BodyDesc.pBigCollisionActivating	= &m_bAttacking;
	BodyDesc.pSmallCollisionActivating	= &m_bAlwaysActivated;

	if (FAILED(__super::Add_PartObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Body_Vindicator"), TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;


	///* 무기를 추가한다. */
	CItem::ITEM_DESC	ItemDesc{};

	CModel* pBody = dynamic_cast<CModel*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Model")));
	if (nullptr == pBody)
		return E_FAIL;

	ItemDesc.strGameObjectTag		= TEXT("GameObject_Axe");
	ItemDesc.strObjectPrototypeTag	= TEXT("Prototype_GameObject_Axe_Steel");
	ItemDesc.strIconTexPrototypeTag = TEXT("Prototype_Component_Texture_Axe_Steel");
	ItemDesc.strPartObjectTag		= TEXT("Part_Weapon_Melee");
	ItemDesc.iDealPoint				= 8;
	ItemDesc.eItemtype				= ITEM_TYPE::MELEE;
	ItemDesc.strIconGameObjectTag	= TEXT("UIGameObject_Axe_Steel");

	ItemDesc.pParentWorldMatrix		= m_pTransformCom->Get_WorldMatrix_Ptr();
	ItemDesc.pState					= &m_iState;
	ItemDesc.pSocketMatrix			= pBody->Get_CombinedTransformationMatrix("J_RWeapon_end");
	ItemDesc.pContainerObject		= this;
	ItemDesc.pBigCollisionActivating = &m_bAttacking;

	if (FAILED(__super::Add_PartObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Axe_Steel"), TEXT("Part_Weapon_Melee"), &ItemDesc)))
		return E_FAIL;

	CItem* pAxe = dynamic_cast<CItem*>(Find_PartObject(TEXT("Part_Weapon_Melee")));
	pAxe->Set_ItemActive(true);


	/* 이펙트를 추가한다. */

	return S_OK;
}

HRESULT CVindicator::Ready_States()
{
	m_StatesVec.resize(static_cast<_uint>(VINDICATOR_STATE::STATE_END));	// state vector 자리 예약

	CModel* pVindicatorModel  = dynamic_cast<CModel*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Model")));
	CCollider* pBigCollider   = dynamic_cast<CCollider*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
	CCollider* pSmallCollider = dynamic_cast<CCollider*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_SmallSphere")));

	CState_Monster::STATEMONSTER_DESC	pStateMonsterDesc = {};
	pStateMonsterDesc.pBigColliderCom	= pBigCollider;
	pStateMonsterDesc.pSmallColliderCom	= pSmallCollider;
	pStateMonsterDesc.pActorModelCom	= pVindicatorModel;
	pStateMonsterDesc.pNavigationCom	= m_pNavigationCom;
	pStateMonsterDesc.pTransformCom		= m_pTransformCom;

	m_StatesVec[static_cast<_uint>(VINDICATOR_STATE::IDLE)]		= CVindicator_Idle::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(VINDICATOR_STATE::WALK)]		= CVindicator_Walk::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(VINDICATOR_STATE::ATTACK)]	= CVindicator_Attack::Create(this, m_pMonsterInfo, &pStateMonsterDesc);
	m_StatesVec[static_cast<_uint>(VINDICATOR_STATE::NOVELTY)]	= CVindicator_Novelty::Create(this, m_pMonsterInfo, &pStateMonsterDesc);

	m_pMonsterFSM = FSM::Create();

	m_pMonsterFSM->Init_State(m_StatesVec[static_cast<_uint>(VINDICATOR_STATE::IDLE)]);

	return S_OK;
}

CVindicator* CVindicator::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVindicator* pGameInstance = new CVindicator(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CVindicator");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CGameObject* CVindicator::Clone(void* pArg)
{
	CVindicator* pGameInstance = new CVindicator(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CVindicator");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CVindicator::Free()
{
	__super::Free();

	for (auto& stateVec : m_StatesVec)
	{
		if (stateVec!= nullptr)
			Safe_Release(stateVec) ;
	}
}
