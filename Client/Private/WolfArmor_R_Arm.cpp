#include "WolfArmor_R_Arm.h"
#include "GameInstance.h"
#include "Item.h"

CWolfArmor_R_Arm::CWolfArmor_R_Arm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{

}

CWolfArmor_R_Arm::CWolfArmor_R_Arm(const CWolfArmor_R_Arm& Prototype)
	: CPartObject(Prototype)
{

}

HRESULT CWolfArmor_R_Arm::Initialize_Prototype()
{
	/* 외부 데이터베이스를 통해서 값을 채운다. */

	return S_OK;
}

HRESULT CWolfArmor_R_Arm::Initialize(void* pArg)
{
	/* 원형의 데이터를 복제하여 사본을 만들고. */
	/* 추가적으로 필요한 데이터를 Arg로 받아와 실 사용하기위한 객체의 정보를 생성해준다. */	

	WOLFARMOR_R_ARM_DESC* pDesc = static_cast<WOLFARMOR_R_ARM_DESC*>(pArg);
	m_bPartActive = pDesc->bPartActive;
	m_pSocketMatrix = pDesc->pRArmSocketMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_strGameObjectTag = TEXT("GameObject_WolfArmor_R_Arm");

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-180.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, -0.1f, 0.f, 1.f));
	
	return S_OK;
}

void CWolfArmor_R_Arm::Priority_Update(_float fTimeDelta)
{
	if (false == m_bPartActive)
		return;
}

void CWolfArmor_R_Arm::Update(_float fTimeDelta)
{
	if (false == m_bPartActive)
		return;
}

void CWolfArmor_R_Arm::Late_Update(_float fTimeDelta)
{
	if (false == m_bPartActive)
		return;

	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)	
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);	

	XMStoreFloat4x4(&m_CombinedWorldMatrix, 
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * 
		SocketMatrix *
		XMLoadFloat4x4(m_pParentWorldMatrix));

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CWolfArmor_R_Arm::Render()
{
	if (false == m_bPartActive)
		return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE, 0)))
			return E_FAIL;	

		if (FAILED(m_pShaderCom->Begin(static_cast<_uint>(0))))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(static_cast<_uint>(i))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CWolfArmor_R_Arm::Ready_Components()
{
	/* Com_Shader */
	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)))
		return E_FAIL;

	/* Com_Model */
	CModel::MODEL_DESC	pModelDesc = {};
	pModelDesc.bPickable = false;

	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_WolfArmor_R_Arm"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &pModelDesc))
		return E_FAIL;

	return S_OK;
}

HRESULT CWolfArmor_R_Arm::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}


CWolfArmor_R_Arm* CWolfArmor_R_Arm::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWolfArmor_R_Arm* pGameInstance = new CWolfArmor_R_Arm(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CWolfArmor_R_Arm");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CGameObject* CWolfArmor_R_Arm::Clone(void* pArg)
{
	CWolfArmor_R_Arm* pGameInstance = new CWolfArmor_R_Arm(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CWolfArmor_R_Arm");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CWolfArmor_R_Arm::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
