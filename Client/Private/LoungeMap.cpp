#include "LoungeMap.h"
#include "GameInstance.h"

CLoungeMap::CLoungeMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject { pDevice, pContext }
{

}

CLoungeMap::CLoungeMap(const CLoungeMap& Prototype)
	: CGameObject { Prototype }
{

}

HRESULT CLoungeMap::Initialize_Prototype()
{
	/* 외부 데이터베이스를 통해서 값을 채운다. */

	return S_OK;
}

HRESULT CLoungeMap::Initialize(void* pArg)
{
	/* 원형의 데이터를 복제하여 사본을 만들고. */
	/* 추가적으로 필요한 데이터를 Arg로 받아와 실 사용하기위한 객체의 정보를 생성해준다. */	
	CGameObject::GAMEOBJECT_DESC		Desc{};

	Desc.pGameObjectTag = TEXT("GameObject_LoungeMap");
	Desc.fSpeedPerSec = 0.f;
	Desc.fRotationPerSec = 0.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(0.f, -91.f, 0.f, 1.f));	

	return S_OK;
}

void CLoungeMap::Priority_Update(_float fTimeDelta)
{

}

void CLoungeMap::Update(_float fTimeDelta)
{

}

void CLoungeMap::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CLoungeMap::Render()
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

HRESULT CLoungeMap::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_LoungeMap"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoungeMap::Bind_ShaderResources()
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

CLoungeMap* CLoungeMap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLoungeMap* pGameInstance = new CLoungeMap(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CLoungeMap");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CGameObject* CLoungeMap::Clone(void* pArg)
{
	CLoungeMap* pGameInstance = new CLoungeMap(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CLoungeMap");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CLoungeMap::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
