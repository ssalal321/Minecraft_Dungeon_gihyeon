#include "InventoryItemSlot.h"
#include "GameInstance.h"

CInventoryItemSlot::CInventoryItemSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject(pDevice, pContext)
{
}

CInventoryItemSlot::CInventoryItemSlot(const CInventoryItemSlot& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT CInventoryItemSlot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInventoryItemSlot::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		m_pDesc = new INVENTORY_ITEMSLOT_DESC(*static_cast<INVENTORY_ITEMSLOT_DESC*>(pArg));
	}
	else
		return E_FAIL;

	if (FAILED(__super::Initialize(m_pDesc)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CInventoryItemSlot::Priority_Update(_float fTimeDelta)
{

}

void CInventoryItemSlot::Update(_float fTimeDelta)
{
}

void CInventoryItemSlot::Late_Update(_float fTimeDelta)
{
}

HRESULT CInventoryItemSlot::Render()
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

HRESULT CInventoryItemSlot::Ready_Components()
{
	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_pDesc->strTextureComTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CInventoryItemSlot* CInventoryItemSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInventoryItemSlot* pGameInstance = new CInventoryItemSlot(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CInventoryItemSlot");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject* CInventoryItemSlot::Clone(void* pArg)
{
	CInventoryItemSlot* pGameInstance = new CInventoryItemSlot(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CInventoryItemSlot");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CInventoryItemSlot::Free()
{
	__super::Free();

	Safe_Delete(m_pDesc);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
