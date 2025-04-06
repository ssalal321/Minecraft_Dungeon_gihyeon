#include "ForkLift.h"
#include "GameInstance.h"

CForkLift::CForkLift(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject { pDevice, pContext }
{

}

CForkLift::CForkLift(const CForkLift& Prototype)
	: CGameObject { Prototype }
{

}

HRESULT CForkLift::Initialize_Prototype()
{
	/* 외부 데이터베이스를 통해서 값을 채운다. */

	return S_OK;
}

HRESULT CForkLift::Initialize(void* pArg)
{
	/* 원형의 데이터를 복제하여 사본을 만들고. */
	/* 추가적으로 필요한 데이터를 Arg로 받아와 실 사용하기위한 객체의 정보를 생성해준다. */	
	CGameObject::GAMEOBJECT_DESC		Desc{};

	Desc.pGameObjectTag = TEXT("GameObject_Terrain");
	Desc.fSpeedPerSec = 0.f;
	Desc.fRotationPerSec = 0.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_pGameInstance->Compute_Random(0.f, 20.f), 
			5.0f, 
			m_pGameInstance->Compute_Random(0.f, 20.f), 
			1.f));	

	return S_OK;
}

void CForkLift::Priority_Update(_float fTimeDelta)
{

}

void CForkLift::Update(_float fTimeDelta)
{

}

void CForkLift::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CForkLift::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
		

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;	

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CForkLift::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ForkLift"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CForkLift::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
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

CForkLift* CForkLift::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CForkLift* pGameInstance = new CForkLift(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CForkLift");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CGameObject* CForkLift::Clone(void* pArg)
{
	CForkLift* pGameInstance = new CForkLift(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CForkLift");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CForkLift::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
