#include "Boss_Trigger.h"
#include "GameInstance.h"
#include "GateFence.h"
#include "Monster.h"
#include "PartObject.h"

CBoss_Trigger::CBoss_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBoss_Trigger::CBoss_Trigger(const CBoss_Trigger& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CBoss_Trigger::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_Trigger::Initialize(void* pArg)
{
	BOSS_TRIGGER_DESC* pDesc = static_cast<BOSS_TRIGGER_DESC*>(pArg);
	m_TriggerPosition = pDesc->triggerPosition;
	m_pMyBoss = pDesc->pBoss;
	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CBoss_Trigger::Priority_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;
}

void CBoss_Trigger::Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;
}

void CBoss_Trigger::Late_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;
}

HRESULT CBoss_Trigger::Render()
{
	if (!m_bActive)
		return S_OK;

	return S_OK;
}


void CBoss_Trigger::Collided_With(CCollider* pOther, CCollider::COLLISION_STATE eCollisionState)
{
	if (CCollider::COLLISION_STATE::ENTER == eCollisionState &&
		TEXT("Player_Body") == pOther->Get_ColliderTag())
	{
		m_pMyBoss->Set_GameObject_Active(true);
		m_pMyBoss->Find_PartObject(TEXT("Part_Body"))->Set_Appearing(true);

		CGameObject* pGateFence = m_pGameInstance->Find_GameObject(TEXT("GameObject_GateFence_0"),
																	m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_BackGround"));
		pGateFence->Set_GameObject_Active(true);
		dynamic_cast<CGateFence*>(pGateFence)->Set_Appearing(true);


		CContainerObject* pPlayer = dynamic_cast<CPartObject*>(pOther->Get_OwnerObject())->Get_ContainerObject();
		CNavigation* pPlayerNav = dynamic_cast<CNavigation*>(pPlayer->Find_Component(TEXT("Com_Navigation")));
		pPlayerNav->Lock_Cell(899);
		pPlayerNav->Lock_Cell(900);
	}
}

HRESULT CBoss_Trigger::Ready_Components()
{
	/* Com_Collider */
	XMStoreFloat4x4(&m_IdentityWorldMatrix, XMMatrixIdentity());

	CBounding_Sphere::BOUNDING_SPHERE_DESC		SphereCollDesc{};

	SphereCollDesc.fRadius		= 1.6f;
	SphereCollDesc.vCenter		= m_TriggerPosition;  // 2.5f, 5.5f, 19.f
	SphereCollDesc.pGameObject	= this;
	SphereCollDesc.CombinedWorldMatrix = &m_IdentityWorldMatrix;
	SphereCollDesc.pCollisionActivated = &m_bActivated;

	CComponent* pColliderSphereCom = Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereCollDesc);

	if (nullptr == pColliderSphereCom)
		return E_FAIL;

	m_pGameInstance->Add_ColliderCom(m_pGameInstance->Get_ChangedLevelIndex(), pColliderSphereCom, TEXT("Boss_Trigger_Collider"), TEXT("Level"));
	dynamic_cast<CCollider*>(pColliderSphereCom)->Set_ColliderRole(CCollider::ETC);

	return S_OK;
}

CBoss_Trigger* CBoss_Trigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CBoss_Trigger* pGameInstance = new CBoss_Trigger(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : CBoss_Trigger");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject* CBoss_Trigger::Clone(void* pArg)
{
	CBoss_Trigger* pGameInstance = new CBoss_Trigger(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CBoss_Trigger");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CBoss_Trigger::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
