#include "Monster.h"

#include "ArrowPool_Monster.h"
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

	m_pNavigationCom->SetUp_CurrentCellIndex(0);

	return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{
	m_pMonsterFSM->Priority_Update_State(fTimeDelta);

	__super::Priority_Update(fTimeDelta);
}

void CMonster::Update(_float fTimeDelta)
{
	m_pNavigationCom->SetUp_On_Navigation(m_pTransformCom);

	m_pMonsterFSM->Update_State(fTimeDelta);

	__super::Update(fTimeDelta);
}

void CMonster::Late_Update(_float fTimeDelta)
{
	m_pMonsterFSM->Late_Update_State(fTimeDelta);

	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);

}

HRESULT CMonster::Render()
{

	return S_OK;
}

#include "MonsterState.h"

void CMonster::Change_State(const MonsterState& state)
{
	switch (state.monsterType)
	{
	case MONSTER_TYPE::ZOMBIE:
		m_iState = static_cast<_uint>(state.ZombieState);
		break;

	case MONSTER_TYPE::SKELETON:
		m_iState = static_cast<_uint>(state.SkeletonState);
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
	_uint	LevelIndex = m_pGameInstance->Get_NextLevelIndex();

	switch (LevelIndex)
	{
	case LEVEL_LOUNGE:
		{
		if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_LoungeMap"),
			TEXT("Com_Navigation_LoungeMap"), reinterpret_cast<CComponent**>(&m_pNavigationCom)))
			return E_FAIL;
		}
		break;
	}

	///* Com_Collider */
	//CBounding_Sphere::BOUNDING_SPHERE_DESC		SphereCollDesc{};

	//SphereCollDesc.fRadius = 1.5f;
	//SphereCollDesc.vCenter = _float3(0.f, SphereCollDesc.fRadius, 0.f);
	//SphereCollDesc.pGameObject = this;
	//SphereCollDesc.CombinedWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	//SphereCollDesc.pCollisionActivated = &m_bHoveringColl;  // 얜 다른 것과는 충돌할 필요 X

	//CComponent* pColliderSphereCom = Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
	//	TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderSphereCom), &SphereCollDesc);

	//if (nullptr == pColliderSphereCom)
	//	return E_FAIL;

	////dynamic_cast<CCollider*>(pColliderSphereCom)->Set_MouseCollider(true);

	//m_pGameInstance->Add_ColliderCom(pColliderSphereCom, TEXT("Monster_Sphere"), TEXT("Monster"));

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

void CMonster::Free()
{
	__super::Free();

	//Safe_Release(m_pColliderSphereCom);
	Safe_Release(m_pNavigationCom);
	Safe_Delete(m_pMonsterInfo);
	Safe_Delete(m_pMonsterFSM);

	for (auto& stateVec : m_StatesVec)
	{
		if (stateVec != nullptr)
			Safe_Release(stateVec);
	}
}
