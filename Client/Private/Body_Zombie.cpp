#include "Body_Zombie.h"
#include "GameInstance.h"
#include "Mesh.h"

#include "Zombie.h"

CBody_Zombie::CBody_Zombie(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{

}

CBody_Zombie::CBody_Zombie(const CBody_Zombie& Prototype)
	: CPartObject(Prototype)
{

}

HRESULT CBody_Zombie::Initialize_Prototype()
{
	/* 외부 데이터베이스를 통해서 값을 채운다. */

	return S_OK;
}

HRESULT CBody_Zombie::Initialize(void* pArg)
{
	BODY_ZOMBIE_DESC* pDesc = static_cast<BODY_ZOMBIE_DESC*>(pArg);

	m_pTargetState = pDesc->pState;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CBody_Zombie::Priority_Update(_float fTimeDelta)
{
}

void CBody_Zombie::Update(_float fTimeDelta)
{
}

void CBody_Zombie::Late_Update(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentWorldMatrix));

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CBody_Zombie::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

void CBody_Zombie::Collided_With(CCollider* pOther, CCollider::COLLISION_STATE eCollisionState)
{
	dynamic_cast<CZombie*>(m_pContainerObject)->Collided_With(pOther, eCollisionState);
}

HRESULT CBody_Zombie::Ready_Components()
{
	/* Com_Shader */
	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)))
		return E_FAIL;

	/* Com_Model */
	CMesh::MESH_DESC	pMeshDesc = {};
	pMeshDesc.bPickable = true;

	if (nullptr == Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Zombie"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &pMeshDesc))
		return E_FAIL;


	///* Com_Collider */
	//CBounding_AABB::BOUNDING_AABB_DESC		AABBCollDesc{};

	//AABBCollDesc.vCenter = _float3(0.f, AABBCollDesc.vExtents.y, 0.f);
	//AABBCollDesc.CombinedWorldMatrix = &m_CombinedWorldMatrix;
	//AABBCollDesc.pGameObject = static_cast<CGameObject*>(this);
	//AABBCollDesc.vExtents = _float3(0.35f, 0.6f, 0.35f);

	//CComponent* pColliderCom = Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
	//	TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderOBB[COLL_AABB]), &AABBCollDesc);

	//if (nullptr == pColliderCom)
	//	return E_FAIL;

	//m_pGameInstance->Add_ColliderCom(pColliderCom, TEXT("Zombie"), TEXT("Monster"));

	///* Com_Collider */
	//CBounding_Sphere::BOUNDING_SPHERE_DESC		SphereCollDesc{};
	//SphereCollDesc.vCenter = _float3(0.f, SphereCollDesc.fRadius, 0.f);
	//SphereCollDesc.CombinedWorldMatrix = &m_CombinedWorldMatrix;
	//SphereCollDesc.pGameObject = static_cast<CGameObject*>(this);
	//SphereCollDesc.fRadius = 0.5f;

	//pColliderCom = Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
	//	TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom[COLL_SPHERE]), &SphereCollDesc);

	//if (nullptr == pColliderCom)
	//	return E_FAIL;

	//m_pGameInstance->Add_ColliderCom(pColliderCom, TEXT("Zombie"), TEXT("Monster"));

	/* Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC		OBBCollDesc{};

	OBBCollDesc.vExtents	= _float3(0.6f, 1.f, 0.6f);
	OBBCollDesc.vCenter		= _float3(0.f, OBBCollDesc.vExtents.y, 0.f);
	OBBCollDesc.vRotation	= _float3(0.f, /*XMConvertToRadians(0.f)*/ 0.f, 0.f);
	OBBCollDesc.pGameObject = this;
	OBBCollDesc.CombinedWorldMatrix = &m_CombinedWorldMatrix;
	OBBCollDesc.pContainerObjAttacking = m_pContainerObjAttacking;
	
	CComponent* pColliderOBBCom = Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderOBBCom), &OBBCollDesc);

	if (nullptr == pColliderOBBCom)
		return E_FAIL;

	m_pGameInstance->Add_ColliderCom(pColliderOBBCom, TEXT("Zombie_OBB"), TEXT("Monster"));

	/* Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC		SphereCollDesc{};

	SphereCollDesc.fRadius = 1.5f;
	SphereCollDesc.vCenter = _float3(0.f, SphereCollDesc.fRadius, 0.f);
	SphereCollDesc.pGameObject = this;
	SphereCollDesc.CombinedWorldMatrix = &m_CombinedWorldMatrix;
	SphereCollDesc.pContainerObjAttacking = m_pContainerObjAttacking;

	CComponent* pColliderSphereCom = Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderSphereCom), &SphereCollDesc);

	if (nullptr == pColliderSphereCom)
		return E_FAIL;

	m_pGameInstance->Add_ColliderCom(pColliderSphereCom, TEXT("Zombie_Sphere"), TEXT("Monster"));

	return S_OK;
}

HRESULT CBody_Zombie::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
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

	if (LIGHT_DESC::TYPE_DIRECTIONAL == pLightDesc->eType)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
			return E_FAIL;
		m_iPassIndex = 0;
	}
	else
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightPos", &pLightDesc->vPosition, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fLightRange", &pLightDesc->fRange, sizeof(_float))))
			return E_FAIL;
		m_iPassIndex = 1;
	}

	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CBody_Zombie* CBody_Zombie::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Zombie* pGameInstance = new CBody_Zombie(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CBody_Zombie");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CGameObject* CBody_Zombie::Clone(void* pArg)
{
	CBody_Zombie* pGameInstance = new CBody_Zombie(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CBody_Zombie");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CBody_Zombie::Free()
{
	__super::Free();

	Safe_Release(m_pColliderOBBCom);
	Safe_Release(m_pColliderSphereCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
