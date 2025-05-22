#include "Monster.h"

#include <random>

#include "MonsterState.h"

#include "GameInstance.h"
#include "FSM.h"
#include "Player.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject ( pDevice, pContext )
{

}

CMonster::CMonster(const CMonster& Prototype)
	: CContainerObject( Prototype )
{

}

HRESULT CMonster::Initialize_Prototype()
{
	/* 외부 데이터베이스를 통해서 값을 채운다. */

	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	/*if (m_pNavigationCom)
		m_pNavigationCom->SetUp_CurrentCellIndex(0);*/

	return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	m_pMonsterFSM->Priority_Update_State(fTimeDelta);
}

void CMonster::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_pNavigationCom && false == m_pTransformCom->Get_Is_Jumping())
	{
		m_pNavigationCom->SetUp_On_Navigation(m_pTransformCom);
	}

	m_pMonsterFSM->Update_State(fTimeDelta);


	if (m_bRenderDamageFont)
	{
		m_fFontRenderedTime += fTimeDelta;

		// 오프셋 누적
		m_vFontOffset.y -= 35.f * fTimeDelta;
		m_vFontCurrentScreenPos.y = m_vFontStartScreenPos.y + m_vFontOffset.y;

		if (m_fFontRenderedTime >= 1.f)
			m_bRenderDamageFont = false;
	}
}

void CMonster::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pMonsterFSM->Late_Update_State(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CMonster::Render()
{
	if (m_bRenderDamageFont)
	{
		std::wstring strHP = std::to_wstring(m_iDealPoint);
		m_pGameInstance->Draw_Text(TEXT("Font_Minecraft"), strHP.c_str(), m_vFontCurrentScreenPos);
	}

	//std::wstring strHP = std::to_wstring(m_pMonsterInfo->Get_CurrentHP());
	//m_pGameInstance->Draw_Text(TEXT("Font_Minecraft"), strHP.c_str(), m_vScreenPos/*, Colors::White, 0.f, { 0.f, 0.f }, 1.f*/);

#ifdef _DEBUG
	if (m_pNavigationCom)
		m_pNavigationCom->Render();
#endif


	return S_OK;
}



void CMonster::Change_State(const MonsterState& state)
{
	switch (state.monsterType)
	{
	case MONSTER_TYPE::ZOMBIE:
		m_iState = static_cast<_uint>(state.ZombieState);
		break;

	case MONSTER_TYPE::BABYZOMBIE:
		m_iState = static_cast<_uint>(state.BabyZombieState);
		break;

	case MONSTER_TYPE::SKELETON:
		m_iState = static_cast<_uint>(state.SkeletonState);
		break;

	case MONSTER_TYPE::SLIME_LARGE:
		m_iState = static_cast<_uint>(state.SlimeLargeState);
		break;

	case MONSTER_TYPE::SLIME_MEDIUM:
		m_iState = static_cast<_uint>(state.SlimeMediumState);
		break;

	case MONSTER_TYPE::SLIME_SMALL:
		m_iState = static_cast<_uint>(state.SlimeSmallState);
		break;

	case MONSTER_TYPE::SLIME_CAULDRON:
		m_iState = static_cast<_uint>(state.SlimeCauldronState);
		break;

	case MONSTER_TYPE::VINDICATOR:
		m_iState = static_cast<_uint>(state.VindicatorState);
		break;

	case MONSTER_TYPE::CAULDRONBOSS:
		m_iState = static_cast<_uint>(state.CauldronBossState);
		break;
	}

	m_pMonsterFSM->Change_State(m_StatesVec[m_iState]);
}


void CMonster::Collided_With(CCollider* pOther, CCollider::COLLISION_STATE eCollisionState)
{
	switch (eCollisionState)
	{
	case CCollider::ENTER:
		m_pMonsterFSM->Collision_Enter(pOther);
		break;

	case CCollider::STAY:
		m_pMonsterFSM->Collision_Stay(pOther);
		break;

	case CCollider::EXIT:
		m_pMonsterFSM->Collision_Exit(pOther);
		break;
	}
}

HRESULT CMonster::Ready_Components()
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
	}

	m_pTransformCom->Set_Navigation(m_pNavigationCom);

	return S_OK;
}

_float4 CMonster::Get_Player_Position(const _wstring& strPlayerGameObjectTag, _uint iPlayerLayerLevelIndex) const
{
	CPlayer*	  pPlayer				= dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject(strPlayerGameObjectTag, iPlayerLayerLevelIndex, TEXT("Layer_Player")));
	if (nullptr == pPlayer)
		return { 0.f, 0.f, 0.f, 1.f };

	CTransform*   pPlayerTransformCom	= dynamic_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));
	_vector		  vPlayerPosition		= pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);

	_float4  playerPos = {0.f, 0.f, 0.f, 0.f};
	XMStoreFloat4(&playerPos, vPlayerPosition);

	return	 playerPos;
}

_vector CMonster::Vec_To_Player(const _wstring& strPlayerGameObjectTag, _uint iPlayerLayerLevelIndex) const
{
	CPlayer*	 pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject(strPlayerGameObjectTag, iPlayerLayerLevelIndex, TEXT("Layer_Player")));
	CTransform*  pPlayerTransformCom = dynamic_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

	_vector		vPlayerPos  = pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector		vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector		dirToPlayer = vPlayerPos - vMonsterPos;
	//dirToPlayer.y = 0.f;

	return	 dirToPlayer;
}

_float CMonster::Length_To_Player() const
{
	_uint	CurrentLevelIndex = m_pGameInstance->Get_CurrentLevelIndex();

	_float   lengthToPlayer = {};
	_vector  vecToPlayer = Vec_To_Player(TEXT("GameObject_Player"), CurrentLevelIndex);
	XMStoreFloat(&lengthToPlayer, XMVector3Length(vecToPlayer));

	return lengthToPlayer;
}


_bool CMonster::Player_In_DetectRange() const
{
	_float	 vecToPlayer = Length_To_Player();

	return	vecToPlayer < m_pMonsterInfo->Get_DetectRange();
}

void CMonster::Render_DamageFont(_int iDealPoint, _float fStartY)
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

	m_vFontCurrentScreenPos.x = m_vFontStartScreenPos.x + m_vFontOffset.x;
	m_vFontCurrentScreenPos.y = m_vFontStartScreenPos.y + m_vFontOffset.y;
}


void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Delete(m_pMonsterInfo);
	Safe_Delete(m_pMonsterFSM);

	for (auto& stateVec : m_StatesVec)
	{
		if (stateVec != nullptr)
			Safe_Release(stateVec);
	}
}
