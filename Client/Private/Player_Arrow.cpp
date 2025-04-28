#include "Player_Arrow.h"
#include "GameInstance.h"

_int  CPlayer_Arrow::m_iArrowID = 0;

CPlayer_Arrow::CPlayer_Arrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer_Arrow::CPlayer_Arrow(const CPlayer_Arrow& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CPlayer_Arrow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_Arrow::Initialize(void* pArg)
{
	const _wstring& arrowGameObjectTag = TEXT("GameObject_Arrow_") + to_wstring(m_iArrowID++);

	m_pArrowDesc = new GAMEOBJECT_DESC(arrowGameObjectTag, 0.f, 15.f);

	if (FAILED(__super::Initialize(m_pArrowDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Arrow::Priority_Update(_float fTimeDelta)
{
	if (false == m_bActive)
		return;
}

void CPlayer_Arrow::Update(_float fTimeDelta)
{
	if (false == m_bActive)
		return;

	if (m_bAttacking)
	{
		m_pTransformCom->Go_Straight(fTimeDelta);

		m_fResetTimer += fTimeDelta;
		if (m_fResetTimer >= 3.5f)
			Reset();
	}
		
	if (m_bCollided)
	{
		m_fResetTimer += fTimeDelta;

		if (m_fResetTimer >= 0.7f)
			Reset();
	}
}

void CPlayer_Arrow::Late_Update(_float fTimeDelta)
{
	if (false == m_bActive)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPlayer_Arrow::Render()
{
	if (false == m_bActive)
		return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(static_cast<_uint>(i))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer_Arrow::Bind_ShaderResources()
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

void CPlayer_Arrow::Shoot(_float4 startPos, _float4 lookPos)
{
	startPos.y += 1.f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&startPos));
	m_pTransformCom->LookAt(XMLoadFloat4(&lookPos));
	m_bAttacking = true;
	m_bActive	 = true;
}

void CPlayer_Arrow::Reset()
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 0.f, 200.f, 0.f, 1.f});
	m_pTransformCom->LookAt({ 0.f, 0.f, 0.f, 1.f });

	m_fResetTimer = 0.f;

	m_bAttacking = false;
	m_bCollided  = false;
	m_bActive	 = false;
}

void CPlayer_Arrow::Collided_With(CCollider* pOther, CCollider::COLLISION_STATE eCollisionState)
{
	if (CCollider::COLLISION_STATE::ENTER == eCollisionState &&
		(TEXT("Monster_Body_Hit") == pOther->Get_ColliderTag() ||
		 TEXT("Monster_Body_NoHit") == pOther->Get_ColliderTag()))
	{
		m_bCollided = true;
		m_fResetTimer = 0.f;

		/*CPartObject* pPartObject = dynamic_cast<CPartObject*>(pOther->Get_OwnerObject());
		CTransform* pOtherTransformCom = dynamic_cast<CTransform*>(pPartObject->Get_ContainerObject()->Find_Component(TEXT("Com_Transform")));

		m_pCurrentParentWorldMatrix = pOtherTransformCom->Get_WorldMatrix_Ptr();*/
	}
}

HRESULT CPlayer_Arrow::Ready_Components()
{
	/* Com_Shader */
	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)))
		return E_FAIL;

	/* Com_Model */
	CModel::MODEL_DESC	pModelDesc = {};
	pModelDesc.bPickable = false;

	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Arrow"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &pModelDesc))
		return E_FAIL;

	/* Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC		OBBCollDesc{};

	OBBCollDesc.vExtents  = _float3(0.2f, 0.2f, 0.6f);
	OBBCollDesc.vCenter   = _float3(0.f, OBBCollDesc.vExtents.y, 0.f);
	OBBCollDesc.vRotation = _float3(0.f, /*XMConvertToRadians(0.f)*/ 0.f, 0.f);
	OBBCollDesc.pGameObject = this;
	OBBCollDesc.CombinedWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	OBBCollDesc.pContainerObjAttacking = &m_bAttacking;

	CComponent* pColliderCom = Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBCollDesc);

	if (nullptr == pColliderCom)
		return E_FAIL;

	m_pGameInstance->Add_ColliderCom(pColliderCom, TEXT("Player_Arrow"), TEXT("Player"));

	return S_OK;
}

CPlayer_Arrow* CPlayer_Arrow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Arrow* pGameInstance = new CPlayer_Arrow(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CPlayer_Arrow");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CGameObject* CPlayer_Arrow::Clone(void* pArg)
{
	CPlayer_Arrow* pGameInstance = new CPlayer_Arrow(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CPlayer_Arrow");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


void CPlayer_Arrow::Free()
{
	__super::Free();

	Safe_Delete(m_pStaticParentWorldMatrix);
	Safe_Delete(m_pArrowDesc);

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
