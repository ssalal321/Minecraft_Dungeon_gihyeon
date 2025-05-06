#include "Player.h"

#include "ArrowPool_Player.h"
#include "GameInstance.h"

#include "Body_Player.h"
#include "Item.h"

#include "FSM.h"
#include "InventoryData.h"
#include "Player_BowAction.h"
#include "Player_GetHit.h"
#include "Player_Glaive_Combo.h"
#include "Player_Idle.h"
#include "Player_Roll.h"
#include "Player_Walk.h"


CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
	: CContainerObject(Prototype)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	/* 외부 데이터베이스를 통해서 값을 채운다. */

	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	m_pPlayerInfo = new PLAYER_DESC(TEXT("GameObject_Player"), 100, 100, 5, 3.f, 40, false, 90.f, 3.f);

	if (FAILED(__super::Initialize(m_pPlayerInfo)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pArrowPool_Player = CArrowPool_Player::Create();
	if (nullptr == m_pArrowPool_Player)	return E_FAIL;

	if (FAILED(Ready_Inventory()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_States()))
		return E_FAIL;

	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));

	if (m_pNavigationCom)
		m_pNavigationCom->SetUp_CurrentCellIndex(0);

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	m_pPlayerFSM->Priority_Update_State(fTimeDelta);

	__super::Priority_Update(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
	if (m_pNavigationCom)
		m_pNavigationCom->SetUp_On_Navigation(m_pTransformCom);

	m_pPlayerFSM->Update_State(fTimeDelta);

	__super::Update(fTimeDelta);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	m_pPlayerFSM->Late_Update_State(fTimeDelta);

	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPlayer::Render()
{
#ifdef _DEBUG
	if (m_pNavigationCom)
		m_pNavigationCom->Render();
#endif

	return S_OK;
}

void CPlayer::Delete_NavigationCom()
{
	Safe_Release(m_pNavigationCom);
}

void CPlayer::Change_State(PLAYER_STATE playerState)
{
	m_iState = static_cast<_uint>(playerState);
	m_pPlayerFSM->Change_State(m_StatesVec[m_iState]);
}

void CPlayer::Collided_With(CCollider* pOther, CCollider::COLLISION_STATE eCollisionState)
{
	switch (eCollisionState)
	{
	case CCollider::ENTER:
		m_pPlayerFSM->Collision_Enter(pOther);
		break;

	case CCollider::STAY:
		m_pPlayerFSM->Collision_Stay(pOther);
		break;

	case CCollider::EXIT:
		m_pPlayerFSM->Collision_Exit(pOther);
		break;
	}
}

HRESULT CPlayer::Ready_Components()
{
	/* Com_Navigation */
	_uint	LevelIndex = m_pGameInstance->Get_ChangedLevelIndex();

	switch (LevelIndex)
	{
	case LEVEL_LOUNGE:
	{
		if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_LoungeMap"),
			TEXT("Com_Navigation_LoungeMap"), reinterpret_cast<CComponent**>(&m_pNavigationCom)))
			return E_FAIL;
	}
	break;

	//case LEVEL_SOGGYSWAMP:
	//{
	//	m_pNavigationCom = nullptr;
	//}
	break;
	}

	return S_OK;
}

HRESULT CPlayer::Ready_Inventory()
{
	m_pInventoryData = CInventoryData::Create(this);

	if (nullptr == m_pInventoryData)
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
	/* 몸통을 추가한다. */
	CBody_Player::BODY_PLAYER_DESC		BodyDesc{};

	BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	BodyDesc.pState = &m_iState;
	BodyDesc.pContainerObject = this;
	BodyDesc.pCollisionActivating = &m_bAttacking;

	if (FAILED(__super::Add_PartObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Body_Player"), TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;


	/* 무기를 추가한다. */
//#pragma region MELEE
//	CItem::ITEM_DESC	ItemDesc{};
//
//	CModel* pBody = dynamic_cast<CModel*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Model")));
//	if (nullptr == pBody)
//		return E_FAIL;
//
//	ItemDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
//	ItemDesc.pState = &m_iState;
//	ItemDesc.pSocketMatrix = pBody->Get_CombinedTransformationMatrix("J_R_Weapon");
//	ItemDesc.pContainerObject = this;
//	ItemDesc.pCollisionActivated = &m_bAttacking;
//
//	if (FAILED(__super::Add_PartObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Glaive_Steel"), TEXT("Part_Weapon_Glaive"), &ItemDesc)))
//		return E_FAIL;
//
//	// 처음엔 콜라이더 끄기
//	CItem* pGlaive = dynamic_cast<CItem*>(Find_PartObject(TEXT("Part_Weapon_Glaive")));
//	CCollider* pWeaponCollider = dynamic_cast<CCollider*>(pGlaive->Find_Component(TEXT("Com_Collider_Sphere")));
//	pWeaponCollider->Set_ColliderActive(false);
//
//	// 인벤토리에 넣기
//	//CItem* pGlaive = dynamic_cast<CItem*>(Find_PartObject(TEXT("Part_Weapon_Melee")));
//	m_pInventoryData->Add_Item_To_StoreSlot(pGlaive);
//#pragma endregion
//
//#pragma region RANGED
//	CItem::ITEM_DESC	BowDesc{};
//
//	BowDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
//	BowDesc.pState = &m_iState;
//	BowDesc.pSocketMatrix = pBody->Get_CombinedTransformationMatrix("J_L_Weapon");
//	BowDesc.pContainerObject = this;
//	BowDesc.pCollisionActivated = &m_bAttacking;
//
//	if (FAILED(__super::Add_PartObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Bow"), TEXT("Part_Weapon_Bow"), &BowDesc)))
//		return E_FAIL;
//
//	CItem* pBow = dynamic_cast<CItem*>(Find_PartObject(TEXT("Part_Weapon_Ranged")));
//	m_pInventoryData->Add_Item_To_StoreSlot(pBow);
//
//#pragma endregion 

	/* 이펙트를 추가한다. */

	return S_OK;
}

HRESULT CPlayer::Ready_States()
{
	m_StatesVec.resize(static_cast<_uint>(PLAYER_STATE::STATE_END));	// state vector 자리 예약

	CModel* pPlayerModel  = dynamic_cast<CModel*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Model")));
	CCollider* pCollider = dynamic_cast<CCollider*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_Sphere")));

	CState_Player::STATEPLAYER_DESC		pStatePlayerDesc = {};
	pStatePlayerDesc.pColliderCom		= pCollider;
	pStatePlayerDesc.pActorModelCom		= pPlayerModel;
	//pStatePlayerDesc.pNavigationCom		= m_pNavigationCom;
	pStatePlayerDesc.pTransformCom		= m_pTransformCom;
	pStatePlayerDesc.pArrowPool_Player	= m_pArrowPool_Player;

	m_StatesVec[static_cast<_uint>(PLAYER_STATE::IDLE)]			 = CPlayer_Idle::Create(this, m_pPlayerInfo, &pStatePlayerDesc);
	m_StatesVec[static_cast<_uint>(PLAYER_STATE::WALK)]			 = CPlayer_Walk::Create(this, m_pPlayerInfo, &pStatePlayerDesc);
	m_StatesVec[static_cast<_uint>(PLAYER_STATE::ROLL)]			 = CPlayer_Roll::Create(this, m_pPlayerInfo, &pStatePlayerDesc);
	m_StatesVec[static_cast<_uint>(PLAYER_STATE::GET_HIT_FRONT)] = CPlayer_GetHit::Create(this, m_pPlayerInfo, &pStatePlayerDesc);
	m_StatesVec[static_cast<_uint>(PLAYER_STATE::GLAIVE_COMBO)]  = CPlayer_Glaive_Combo::Create(this, m_pPlayerInfo, &pStatePlayerDesc);
	m_StatesVec[static_cast<_uint>(PLAYER_STATE::BOW_ACTION)]	 = CPlayer_BowAction::Create(this, m_pPlayerInfo, &pStatePlayerDesc);

	m_pPlayerFSM = FSM::Create();

	m_pPlayerFSM->Init_State(m_StatesVec[static_cast<_uint>(PLAYER_STATE::IDLE)]);

	return S_OK;
}


CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pGameInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CPlayer");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pGameInstance = new CPlayer(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CPlayer");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Delete(m_pPlayerInfo);
	Safe_Release(m_pInventoryData);
	Safe_Release(m_pArrowPool_Player);
	Safe_Delete(m_pPlayerFSM);
	Safe_Release(m_pNavigationCom);

	for (auto& stateVec : m_StatesVec)
	{
		if (stateVec!= nullptr)
			Safe_Release(stateVec);
	}
}
