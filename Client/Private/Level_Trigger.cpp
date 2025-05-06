#include "Level_Trigger.h"
#include "GameInstance.h"

CLevel_Trigger::CLevel_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CLevel_Trigger::CLevel_Trigger(const CLevel_Trigger& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CLevel_Trigger::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLevel_Trigger::Initialize(void* pArg)
{
	LEVEL_TRIGGER_DESC* pDesc = static_cast<LEVEL_TRIGGER_DESC*>(pArg);
	m_TriggerPosition = pDesc->triggerPosition;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Trigger::Priority_Update(_float fTimeDelta)
{
	if (false == m_bActive)
		return;
}

void CLevel_Trigger::Update(_float fTimeDelta)
{
	if (false == m_bActive)
		return;

}

void CLevel_Trigger::Late_Update(_float fTimeDelta)
{
	if (false == m_bActive)
		return;

	//m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CLevel_Trigger::Render()
{
	if (false == m_bActive)
		return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Trigger::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Trigger::Collided_With(CCollider* pOther, CCollider::COLLISION_STATE eCollisionState)
{
	if (CCollider::COLLISION_STATE::ENTER == eCollisionState &&
		TEXT("Player_Body") == pOther->Get_ColliderTag())
	{
		m_bSceneChanged = true;
	}
}

HRESULT CLevel_Trigger::Ready_Components()
{
	/* Com_Collider */
	XMStoreFloat4x4(&m_IdentityWorldMatrix, XMMatrixIdentity());

	CBounding_Sphere::BOUNDING_SPHERE_DESC		SphereCollDesc{};

	SphereCollDesc.fRadius = 1.6f;
	SphereCollDesc.vCenter = m_TriggerPosition;  // 2.5f, 5.5f, 19.f
	SphereCollDesc.pGameObject = this;
	SphereCollDesc.CombinedWorldMatrix = &m_IdentityWorldMatrix;
	SphereCollDesc.pCollisionActivated = &m_bActivated;

	CComponent* pColliderSphereCom = Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereCollDesc);

	if (nullptr == pColliderSphereCom)
		return E_FAIL;

	m_pGameInstance->Add_ColliderCom(m_pGameInstance->Get_ChangedLevelIndex(), pColliderSphereCom, TEXT("Level_Change_Collider"), TEXT("Level"));

	return S_OK;
}

CLevel_Trigger* CLevel_Trigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CLevel_Trigger* pGameInstance = new CLevel_Trigger(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : CLevel_Trigger");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject* CLevel_Trigger::Clone(void* pArg)
{
	CLevel_Trigger* pGameInstance = new CLevel_Trigger(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CLevel_Trigger");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CLevel_Trigger::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
