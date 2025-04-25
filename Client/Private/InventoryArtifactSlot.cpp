#include "InventoryArtifactSlot.h"
#include "GameInstance.h"

CInventoryArtifactSlot::CInventoryArtifactSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInventorySlot(pDevice, pContext)
{
}

CInventoryArtifactSlot::CInventoryArtifactSlot(const CInventoryArtifactSlot& Prototype)
	: CInventorySlot(Prototype)
{
}

HRESULT CInventoryArtifactSlot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInventoryArtifactSlot::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		m_pDesc = new INVENTORY_SLOT_DESC(*static_cast<INVENTORY_SLOT_DESC*>(pArg));
	}
	else
		return E_FAIL;

	if (FAILED(__super::Initialize(m_pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CInventoryArtifactSlot::Priority_Update(_float fTimeDelta)
{
}

void CInventoryArtifactSlot::Update(_float fTimeDelta)
{
}

void CInventoryArtifactSlot::Late_Update(_float fTimeDelta)
{
}

HRESULT CInventoryArtifactSlot::Render()
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

HRESULT CInventoryArtifactSlot::Ready_Components()
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
	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

CInventoryArtifactSlot* CInventoryArtifactSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInventoryArtifactSlot* pGameInstance = new CInventoryArtifactSlot(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CInventoryArtifactSlot");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject* CInventoryArtifactSlot::Clone(void* pArg)
{
	CInventoryArtifactSlot* pGameInstance = new CInventoryArtifactSlot(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CInventoryArtifactSlot");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CInventoryArtifactSlot::Free()
{
	__super::Free();
}
