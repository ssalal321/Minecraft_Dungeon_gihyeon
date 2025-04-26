#include "InventoryStoreSlot.h"
#include "GameInstance.h"
CInventoryStoreSlot::CInventoryStoreSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInventorySlot(pDevice, pContext)
{
}

CInventoryStoreSlot::CInventoryStoreSlot(const CInventoryStoreSlot& Prototype)
	: CInventorySlot(Prototype)
{
}

HRESULT CInventoryStoreSlot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInventoryStoreSlot::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		m_pDesc = new INVENTORY_SLOT_DESC(*static_cast<INVENTORY_SLOT_DESC*>(pArg));
	}
	else
		return E_FAIL;

	m_eSlotType = SLOT_TYPE::STORE;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CInventoryStoreSlot::Priority_Update(_float fTimeDelta)
{

}

void CInventoryStoreSlot::Update(_float fTimeDelta)
{
}

void CInventoryStoreSlot::Late_Update(_float fTimeDelta)
{
}

HRESULT CInventoryStoreSlot::Render()
{
	if (!Is_Visible())
		return S_OK;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(Bind_ShaderMatrices(m_pShaderCom, "g_ViewMatrix", "g_ProjMatrix")))
		return E_FAIL;


	if (FAILED(m_pSlotTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	m_pVIBufferCom->Input_Assembler();
	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CInventoryStoreSlot::Ready_Components()
{
	/* Com_Texture */
	if (nullptr == Add_Component(LEVEL_STATIC, m_pDesc->strTexPrototypeTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pSlotTextureCom)))
		return E_FAIL;

	/* Com_Shader */
	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if(nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

CInventoryStoreSlot* CInventoryStoreSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInventoryStoreSlot* pGameInstance = new CInventoryStoreSlot(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CInventoryStoreSlot");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject* CInventoryStoreSlot::Clone(void* pArg)
{
	CInventoryStoreSlot* pGameInstance = new CInventoryStoreSlot(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CInventoryStoreSlot");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CInventoryStoreSlot::Free()
{
	__super::Free();
}
