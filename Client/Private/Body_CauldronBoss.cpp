#include "Body_CauldronBoss.h"

#include "CauldronBoss.h"
#include "GameInstance.h"
#include "Mesh.h"
#include "UIObject.h"

CBody_CauldronBoss::CBody_CauldronBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{

}

CBody_CauldronBoss::CBody_CauldronBoss(const CBody_CauldronBoss& Prototype)
	: CPartObject(Prototype)
{

}

HRESULT CBody_CauldronBoss::Initialize_Prototype()
{
	/* 외부 데이터베이스를 통해서 값을 채운다. */

	return S_OK;
}

HRESULT CBody_CauldronBoss::Initialize(void* pArg)
{
	BODY_CAULDRONBOSS_DESC* pDesc = static_cast<BODY_CAULDRONBOSS_DESC*>(pArg);

	m_pTargetState = pDesc->pState;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fAppearDuration = 5.f;
	m_fDeathDuration  = 5.f;

	m_bActive = false;

	return S_OK;
}

void CBody_CauldronBoss::Priority_Update(_float fTimeDelta)
{

}

void CBody_CauldronBoss::Update(_float fTimeDelta)
{
	if (m_bAppearing)
	{
		m_fAppearTime += fTimeDelta;
		if (m_fAppearTime >= m_fAppearDuration)
		{
			m_bAppearing	  = false;
			m_fAppearTime = m_fAppearDuration;
			dynamic_cast<CCauldronBoss*>(m_pContainerObject)->Get_My_HPUIObject()->Set_GameObject_Active(true);  // 체력바 보이기
		}
	}

	if (m_bDying)
	{
		m_fDeathTime += fTimeDelta;
		if (m_fDeathTime >= m_fDeathDuration)
		{
			m_bDying = false;
			m_fDeathTime = m_fDeathDuration;
			dynamic_cast<CCauldronBoss*>(m_pContainerObject)->Get_My_HPUIObject()->Set_GameObject_Active(false);  // 체력바 숨기기
		}
	}

}

void CBody_CauldronBoss::Late_Update(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentWorldMatrix));

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
}

HRESULT CBody_CauldronBoss::Render()
{
	_uint iPassIndex = {};
	if (m_bDying)
		iPassIndex = 2; // Disappear
	else if (m_bAppearing)
		iPassIndex = 0; // Appear
	else
		iPassIndex = 1; // Default

	if (FAILED(Bind_ShaderResources(iPassIndex)))
		return E_FAIL;


	_uint	iNumMeshes	= m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(iPassIndex)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(static_cast<_uint>(i))))
			return E_FAIL;
	}

	return S_OK;
}

void CBody_CauldronBoss::Collided_With(CCollider* pOther, CCollider::COLLISION_STATE eCollisionState)
{
	dynamic_cast<CCauldronBoss*>(m_pContainerObject)->Collided_With(pOther, eCollisionState);
}

HRESULT CBody_CauldronBoss::Ready_Components()
{
	/* Com_Shader */
	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_BossAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)))
		return E_FAIL;

	/* Com_Model */
	CModel::MODEL_DESC	pModelDesc = {};
	pModelDesc.bPickable = true;

	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_CauldronBoss"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &pModelDesc))
		return E_FAIL;

	/* Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC		SphereCollDesc{};

	SphereCollDesc.fRadius = 4.f;
	SphereCollDesc.vCenter = _float3(0.f, SphereCollDesc.fRadius, 0.f);
	SphereCollDesc.pGameObject = this;
	SphereCollDesc.CombinedWorldMatrix = &m_CombinedWorldMatrix;
	SphereCollDesc.pCollisionActivated = m_pBigCollisionActivating;

	CComponent* pColliderBigSphereCom = Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_BigSphere"), reinterpret_cast<CComponent**>(&m_pBigColliderCom), &SphereCollDesc);

	if (nullptr == pColliderBigSphereCom)
		return E_FAIL;

	m_pGameInstance->Add_ColliderCom(m_pGameInstance->Get_ChangedLevelIndex(), m_pBigColliderCom, TEXT("Boss_Body_NoHit"), TEXT("Monster"));
	dynamic_cast<CCollider*>(pColliderBigSphereCom)->Set_ColliderRole(CCollider::BIG);


	/* Com_Collider Small*/
	CBounding_Sphere::BOUNDING_SPHERE_DESC		SphereSmallCollDesc{};

	SphereSmallCollDesc.fRadius = 3.2f;
	SphereSmallCollDesc.vCenter = _float3(0.f, SphereSmallCollDesc.fRadius + 0.5f, 0.f);
	SphereSmallCollDesc.pGameObject = this;
	SphereSmallCollDesc.CombinedWorldMatrix = &m_CombinedWorldMatrix;
	SphereSmallCollDesc.pCollisionActivated = m_pSmallCollisionActivating;

	CComponent* pColliderSmallSphereCom = Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_SmallSphere"), reinterpret_cast<CComponent**>(&m_pSmallColliderCom), &SphereSmallCollDesc);

	if (nullptr == pColliderSmallSphereCom)
		return E_FAIL;

	m_pGameInstance->Add_ColliderCom(m_pGameInstance->Get_ChangedLevelIndex(), m_pSmallColliderCom, TEXT("Boss_Body_Small"), TEXT("Monster"));
	dynamic_cast<CCollider*>(pColliderSmallSphereCom)->Set_AllowSameGroupCollision(true);
	dynamic_cast<CCollider*>(pColliderSmallSphereCom)->Set_ColliderRole(CCollider::SMALL);

	return S_OK;
}

HRESULT CBody_CauldronBoss::Bind_ShaderResources(_uint iPassIndex)
{
	// 공통 바인딩
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix))) return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4)))) return E_FAIL;

	// 조명 바인딩
	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if (!pLightDesc) return E_FAIL;

	if (pLightDesc->eType == LIGHT_DESC::TYPE_DIRECTIONAL)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4)))) return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightPos", &pLightDesc->vPosition, sizeof(_float4)))) return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fLightRange", &pLightDesc->fRange, sizeof(_float)))) return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4)))) return E_FAIL;

	// ===== 패스별 바인딩 =====
	switch (iPassIndex)
	{
	case 0: // Appear
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAppearTime", &m_fAppearTime, sizeof(_float)))) return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAppearDuration", &m_fAppearDuration, sizeof(_float)))) return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bAppearing", &m_bAppearing, sizeof(_bool)))) return E_FAIL;
		break;

	case 2: // Disappear
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bDying", &m_bDying, sizeof(_bool)))) return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDeathTime", &m_fDeathTime, sizeof(_float)))) return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDeathDuration", &m_fDeathDuration, sizeof(_float)))) return E_FAIL;
		break;

	case 1: // Default
	default:
		// 아무것도 안 해도 됨 (g_bAppearing = false 상태가 기본)
		break;
	}

	return S_OK;
}


CBody_CauldronBoss* CBody_CauldronBoss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_CauldronBoss* pGameInstance = new CBody_CauldronBoss(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CBody_CauldronBoss");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CGameObject* CBody_CauldronBoss::Clone(void* pArg)
{
	CBody_CauldronBoss* pGameInstance = new CBody_CauldronBoss(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CBody_CauldronBoss");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CBody_CauldronBoss::Free()
{
	__super::Free();

	Safe_Release(m_pBigColliderCom);
	Safe_Release(m_pSmallColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
