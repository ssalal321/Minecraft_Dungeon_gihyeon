#include "Sky.h"
#include "GameInstance.h"

CSky::CSky(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject { pDevice, pContext }
{

}

CSky::CSky(const CSky& Prototype)
	: CGameObject { Prototype }
{

}

HRESULT CSky::Initialize_Prototype()
{
	/* 외부 데이터베이스를 통해서 값을 채운다. */

	return S_OK;
}

HRESULT CSky::Initialize(void* pArg)
{
	/* 원형의 데이터를 복제하여 사본을 만들고. */
	/* 추가적으로 필요한 데이터를 Arg로 받아와 실 사용하기위한 객체의 정보를 생성해준다. */	
	CGameObject::GAMEOBJECT_DESC		Desc{};

	Desc.pGameObjectTag = TEXT("GameObject_Sky");
	Desc.fSpeedPerSec = 0.f;
	Desc.fRotationPerSec = 0.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	return S_OK;
}

void CSky::Priority_Update(_float fTimeDelta)
{

}

void CSky::Update(_float fTimeDelta)
{

}

void CSky::Last_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CSky::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Input_Assembler()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
		
	return S_OK;
}

HRESULT CSky::Ready_Components()
{
	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxCube"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSky::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 2)))
		return E_FAIL;

	return S_OK;
}

CSky* CSky::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSky* pGameInstance = new CSky(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CSky");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CGameObject* CSky::Clone(void* pArg)
{
	CSky* pGameInstance = new CSky(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CSky");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CSky::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
