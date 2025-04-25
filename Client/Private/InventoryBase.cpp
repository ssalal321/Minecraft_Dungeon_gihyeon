#include "InventoryBase.h"
#include "GameInstance.h"
#include "Item.h"
#include "InventoryArtifactSlot.h"
#include "InventoryGearSlot.h"
#include "InventoryStoreSlot.h"

CInventoryBase::CInventoryBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject(pDevice, pContext)
{
}

CInventoryBase::CInventoryBase(const CInventoryBase& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT CInventoryBase::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInventoryBase::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		m_pDesc = new INVENTORY_BASE_DESC(*static_cast<INVENTORY_BASE_DESC*>(pArg));
	}
	else
		return E_FAIL;

	m_UIStoreSlots.resize(STORESLOTSIZE, nullptr);
	m_UIGearSlots.resize(GEARSLOTSIZE, nullptr);
	m_UIArtifactSlots.resize(ARTIFACTSLOTSIZE, nullptr);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pGameInstance->Subscribe<Item_Added>(
		[this](const Item_Added& evt) { this->Item_Added_To_StoreSlot(evt); }
	);

	return S_OK;
}

void CInventoryBase::Priority_Update(_float fTimeDelta)
{

}

void CInventoryBase::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down('I'))
	{
		m_bVisible = !m_bVisible;
	}
}

void CInventoryBase::Late_Update(_float fTimeDelta)
{
}

HRESULT CInventoryBase::Render()
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


HRESULT CInventoryBase::Ready_Components()
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

void CInventoryBase::Item_Added_To_StoreSlot(const Item_Added& evt)
{
	if (false == m_UIStoreSlots[evt.slotIndex]->Is_Empty() ||
		evt.slotIndex < 0 || evt.slotIndex >= m_UIStoreSlots.size())
		return;

	CInventoryStoreSlot* pInventoryStoreSlot = m_UIStoreSlots[evt.slotIndex];
	pInventoryStoreSlot->Set_Empty(false);
	pInventoryStoreSlot->Set_IconTag(evt.m_strIconTexPrototypeTag);
}

void CInventoryBase::Update_SlotTexture(CInventorySlot* pSlot, const _wstring& texTag, _bool bEmpty)
{
	if (!pSlot)
		return;

	if (bEmpty)
		//TexComponent_Change();

	pSlot->Set_IconTag(texTag);
	// 여기에 pSlot->m_pIcon 등 실제 Texture 변경 로직도 들어가야 함
}

CInventoryBase* CInventoryBase::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInventoryBase* pGameInstance = new CInventoryBase(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CInventoryBase");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject* CInventoryBase::Clone(void* pArg)
{
	CInventoryBase* pGameInstance = new CInventoryBase(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CInventoryBase");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CInventoryBase::Free()
{
	__super::Free();

	for (auto& storeSlot : m_UIStoreSlots)
	{
		Safe_Release(storeSlot);
	}
	m_UIStoreSlots.clear();

	for (auto& gearSlot : m_UIGearSlots)
	{
		Safe_Release(gearSlot);
	}
	m_UIGearSlots.clear();

	for (auto& artifactSlot : m_UIArtifactSlots)
	{
		Safe_Release(artifactSlot);
	}
	m_UIArtifactSlots.clear();

	Safe_Delete(m_pDesc);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}