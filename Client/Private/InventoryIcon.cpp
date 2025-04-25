#include "InventoryIcon.h"
#include "GameInstance.h"

CInventoryIcon::CInventoryIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject(pDevice, pContext)
{
}

HRESULT CInventoryIcon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInventoryIcon::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		m_pDesc = new INVENTORY_ICON_DESC(*static_cast<INVENTORY_ICON_DESC*>(pArg));
	}
	else
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CInventoryIcon::Priority_Update(_float fTimeDelta)
{
}

void CInventoryIcon::Update(_float fTimeDelta)
{
}

void CInventoryIcon::Late_Update(_float fTimeDelta)
{
	
}

HRESULT CInventoryIcon::Render()
{
	if (!Is_Visible())
		return S_OK;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(Bind_ShaderMatrices(m_pShaderCom, "g_ViewMatrix", "g_ProjMatrix")))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	m_pVIBufferCom->Input_Assembler();
	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CInventoryIcon::Ready_Components()
{
	/* Com_Texture */
	if (nullptr == Add_Component(LEVEL_STATIC, m_pDesc->strTexPrototypeTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom)))
		return E_FAIL;

	/* Com_Shader */
	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

CInventoryIcon* CInventoryIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInventoryIcon* pGameInstance = new CInventoryIcon(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CInventoryIcon");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject* CInventoryIcon::Clone(void* pArg)
{
	CInventoryIcon* pGameInstance = new CInventoryIcon(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CInventoryIcon");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CInventoryIcon::Free()
{
	__super::Free();

	Safe_Delete(m_pDesc);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}