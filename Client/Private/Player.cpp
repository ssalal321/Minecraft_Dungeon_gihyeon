#include "Player.h"

#include <iostream>
#include <random>

#include "Armor.h"
#include "ArrowPool_Player.h"
#include "GameInstance.h"

#include "Body_Player.h"
#include "Item.h"

#include "FSM.h"
#include "InventoryData.h"
#include "Monster.h"
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
	m_pPlayerInfo = new PLAYER_DESC(TEXT("GameObject_Player"), 3000, 3000, 15, 4.f, 0.f, 5.f, 0.f, 40.f, 100, false, 90.f, 3.f);

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
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-11.f, 0.05f, -2.5f, 1.f));

	m_ArrowScreenPos = { g_iWinSizeX * 0.5f + 217.f, g_iWinSizeY - 105.f * 0.5f - 23.f };

	if (m_pNavigationCom)
		m_pNavigationCom->SetUp_CurrentCellIndex(1);

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	m_pPlayerFSM->Priority_Update_State(fTimeDelta);	
}

void CPlayer::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_pNavigationCom)
		m_pNavigationCom->SetUp_On_Navigation(m_pTransformCom);

	if (m_pPlayerInfo->Get_RollCoolDowning())
		m_pPlayerInfo->Roll_CoolDown(fTimeDelta);

	if (m_pPlayerInfo->Get_HPCoolDowning())
		m_pPlayerInfo->HP_CoolDown(fTimeDelta);

	if (m_bRenderDamageFont)
	{
		m_fFontRenderedTime += fTimeDelta;

		// 오프셋 누적
		m_vFontOffset.y -= 35.f * fTimeDelta;
		m_FontCurrentScreenPos.y = m_vFontStartScreenPos.y + m_vFontOffset.y;

		if (m_fFontRenderedTime >= 1.f)
			m_bRenderDamageFont = false;
	}

	m_pPlayerFSM->Update_State(fTimeDelta);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pPlayerFSM->Late_Update_State(fTimeDelta);

	Hover_and_Chase_Monster();

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPlayer::Render()
{
	if (m_bRenderDamageFont)
	{
		std::wstring strHP = std::to_wstring(m_iDealPoint);
		m_pGameInstance->Draw_Text(TEXT("Font_Minecraft"), strHP.c_str(), m_FontCurrentScreenPos);
	}

	std::wstring strArrowNum = std::to_wstring(m_pPlayerInfo->Get_ArrowNum());
	m_pGameInstance->Draw_Text(TEXT("Font_Minecraft"), strArrowNum.c_str(), m_ArrowScreenPos);

#ifdef _DEBUG
	if (m_pNavigationCom)
		m_pNavigationCom->Render();
#endif

	return S_OK;
}

void CPlayer::Hover_and_Chase_Monster()
{
	_float4     fWorldMousePos = {};
	_float3     fWorldMouseRay = {};
	m_pGameInstance->Compute_MouseRay(fWorldMousePos, fWorldMouseRay);

	// 1. 현재 가장 가까운 Monster collider 찾기
	CCollider* pClosestCollider = Get_Closest_Collider(fWorldMousePos, fWorldMouseRay);
	if (nullptr == pClosestCollider || false == pClosestCollider->Get_ColliderActive())
		return;

	CMonster* pPrevMonster = m_pPickedMonster;
	CMonster* pCurrMonster = dynamic_cast<CMonster*>(dynamic_cast<CPartObject*>(pClosestCollider->Get_OwnerObject())->Get_ContainerObject());

	// 2. 이전 Hovered 상태 해제
	if (pPrevMonster && pPrevMonster != pCurrMonster)
	{
		pPrevMonster->Set_Hovered(false);

		//std::wcerr << "[휘바 끝XXXXXXXXXXX]" << std::endl;
	}

	// 3. 현재 Hovered 상태 설정 및 클릭 처리
	if (pCurrMonster)
	{
		pCurrMonster->Set_Hovered(true);
		m_pPickedMonster = pCurrMonster;

		//std::wcerr << "[휘바휘바]" << std::endl;

		if (m_pGameInstance->Get_Key(VK_LBUTTON) && !bMouseClickLock)
		{
			Click_Chase_Monster(pCurrMonster);
		}
	}

	if (m_pGameInstance->Key_Up(VK_LBUTTON) && !bMouseClickLock)
	{
		Set_Chasing(false);
	}
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

void CPlayer::Render_DamageFont(_int iDealPoint, _float fStartY)
{
	m_iDealPoint = iDealPoint;
	m_bRenderDamageFont = true;
	m_fFontRenderedTime = 0.f;

	_vector vWorldPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, fStartY, 0.f, 1.f);

	XMStoreFloat2(&m_vFontStartScreenPos,
		XMVector3Project(
			vWorldPos,
			0.f, 0.f,
			static_cast<_float>(g_iWinSizeX),
			static_cast<_float>(g_iWinSizeY),
			0.f, 1.f,
			m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ),
			m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW),
			XMMatrixIdentity()));

	m_vFontOffset = { 0.f, 0.f };

	// 더 정교한 랜덤 흔들림
	std::random_device rd;
	std::mt19937 gen(rd());
	uniform_real_distribution<_float> dist(-3.0f, 3.0f);
	m_vFontOffset.x += dist(gen);

	m_FontCurrentScreenPos.x = m_vFontStartScreenPos.x + m_vFontOffset.x;
	m_FontCurrentScreenPos.y = m_vFontStartScreenPos.y + m_vFontOffset.y;
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
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom)))
			return E_FAIL;
	}
	break;

	case LEVEL_SOGGYSWAMP:
	{
		if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_SoggySwampMap"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom)))
			return E_FAIL;
	}
	break;
	}

	m_pTransformCom->Set_Navigation(m_pNavigationCom);  //  씬 전환해도 덮어쓰일 것

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

	BodyDesc.pParentWorldMatrix			= m_pTransformCom->Get_WorldMatrix_Ptr();
	BodyDesc.pState						= &m_iState;
	BodyDesc.pContainerObject			= this;
	BodyDesc.pBigCollisionActivating	= &m_bAlwaysNotActivated;
	BodyDesc.pSmallCollisionActivating	= &m_bAlwaysActivated;

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
	CCollider* pColliderBig   = dynamic_cast<CCollider*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_BigSphere")));
	CCollider* pColliderSmall = dynamic_cast<CCollider*>(Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Collider_SmallSphere")));

	CState_Player::STATEPLAYER_DESC		pStatePlayerDesc = {};
	pStatePlayerDesc.pColliderBigCom	= pColliderBig;
	pStatePlayerDesc.pColliderSmallCom	= pColliderSmall;
	pStatePlayerDesc.pActorModelCom		= pPlayerModel;
	//pStatePlayerDesc.pNavigationCom	= m_pNavigationCom;
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

CCollider* CPlayer::Get_Closest_Collider(const _float4& mousePos, const _float3& mouseRay)
{
	unordered_map<_wstring, vector<CCollider*>> colliders = *m_pGameInstance->Get_Colliders(m_pGameInstance->Get_CurrentLevelIndex());
	auto it = colliders.find(TEXT("Monster"));
	if (it == colliders.end())
		return nullptr;

	CCollider* pClosest = nullptr;
	_float minDist = FLT_MAX;

	for (auto& pCollider : it->second)
	{
		if (pCollider->Get_ColliderType() != COLLIDER_TYPE::TYPE_SPHERE || CCollider::COLLIDER_ROLE::BIG != pCollider->Get_Role())
			continue;

		_float fDist = 0.f;
		CBounding_Sphere::RayDesc rayDesc = {};
		rayDesc.MousePos = { mousePos.x, mousePos.y, mousePos.z };
		rayDesc.MouseRay = mouseRay;
		rayDesc.fDist = &fDist;

		if (pCollider->Get_Bounding()->Intersect(COLLIDER_TYPE::TYPE_RAY, nullptr, &rayDesc))
		{
			if (fDist < minDist)
			{
				minDist = fDist;
				pClosest = pCollider;
			}
		}
	}

	return pClosest;
}

void CPlayer::Click_Chase_Monster(CMonster* pMonster)
{
	if (!pMonster) return;

	_float4 monsterPickedPos = { 0.f, 0.f, 0.f, 1.f };

	CTransform* pMonsterTransform = dynamic_cast<CTransform*>(pMonster->Find_Component(TEXT("Com_Transform")));
	if (!pMonsterTransform) return;

	Set_Chasing(true, pMonsterTransform);

	XMStoreFloat4(&monsterPickedPos, pMonsterTransform->Get_State(CTransform::STATE_POSITION));
	Set_MonsterPickedPos(monsterPickedPos);
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
