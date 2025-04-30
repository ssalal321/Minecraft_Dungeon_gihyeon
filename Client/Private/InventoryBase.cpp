#include "InventoryBase.h"
#include "GameInstance.h"
#include "Item.h"
#include "InventoryArtifactSlot.h"
#include "InventoryGearSlot.h"
#include "InventoryIcon.h"
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

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_UIStoreSlots.resize(STORESLOTSIZE, nullptr);
	m_UIGearSlots.resize(GEARSLOTSIZE, nullptr);
	m_UIArtifactSlots.resize(ARTIFACTSLOTSIZE, nullptr);

	
	if (FAILED(Ready_Subscribe_Events()))
		return E_FAIL;

	if (FAILED(Ready_UISlots()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

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
		bMouseClickLock = !bMouseClickLock;
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


HRESULT CInventoryBase::Ready_Subscribe_Events()
{
	if (FAILED(m_pGameInstance->Subscribe<Item_Added_To_StoreSlot>(
		[this](const Item_Added_To_StoreSlot& item_AddedEvent) { this->Add_Icon_To_StoreSlot(item_AddedEvent); })))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Subscribe<Unequipped_To_StoreSlot>(
		[this](const Unequipped_To_StoreSlot& item_Unequipped_To_StoreEvent) { this->Unequip_Icon_To_StoreSlot(item_Unequipped_To_StoreEvent); })))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Subscribe<Swap_Store_with_Gear>(
		[this](const Swap_Store_with_Gear& swap_Store_GearEvent) { this->Swap_Store_and_Gear_Icon(swap_Store_GearEvent); })))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Subscribe<Swap_Store_with_Artifact>(
		[this](const Swap_Store_with_Artifact& swap_Store_ArtifactEvent) { this->Swap_Store_and_Artifact_Icon(swap_Store_ArtifactEvent); })))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Subscribe<Equip_To_Gear>(
		[this](const Equip_To_Gear& equip_To_GearEvent) { this->Equip_Icon_To_GearSlot(equip_To_GearEvent); })))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Subscribe<Equip_To_Artifact>(
		[this](const Equip_To_Artifact& equip_To_ArtifactEvent) { this->Equip_Icon_To_ArtifactSlot(equip_To_ArtifactEvent); })))
		return E_FAIL;

	m_bReadyForEvents = true;
	return S_OK;
}

HRESULT CInventoryBase::Ready_UISlots()
{
	const _float fStartX = 513.f; // 첫 번째 열의 X 좌표 시작점
	const _float fStartY = 188.f; // 첫 번째 행의 Y 좌표 시작점
	const _float fRightSlotWidth = 102.5f; // 슬롯의 너비
	const _float fStoreSlotSpacing = 10.5f;  // 슬롯 간의 간격

	const _int iColumns = 3;     // 한 행의 열 수 (3열)
	const _int iRows = 4;     // 한 열의 행 수 (4행)

	for (_int i = 0; i < iRows * iColumns; ++i)
	{
		_int row = i / iColumns;  // 행 계산
		_int col = i % iColumns;  // 열 계산

		// X, Y 좌표 계산
		_float fXPosition = fStartX + (fRightSlotWidth + fStoreSlotSpacing) * static_cast<_float>(col);   // 열에 맞게 X 좌표 계산
		_float fYPosition = fStartY + (fRightSlotWidth + fStoreSlotSpacing) * static_cast<_float>(row);  // 행에 맞게 Y 좌표 계산

		const _wstring& strSlotName = L"GameObject_InventoryStoreSlot_" + std::to_wstring(i);

		CInventoryBase::INVENTORY_BASE_DESC  InventoryStoreSlotDesc
		(strSlotName.c_str(), CUIObject::CLICKABLE,
			fXPosition, fYPosition, 0.5f, fRightSlotWidth, fRightSlotWidth,
			L"Prototype_Component_Texture_InventoryStoreSlot");

		CUIObject* pInventoryStoreSlot = m_pGameInstance->Add_UIObject(LEVEL_STATIC, LEVEL_GAMEPLAY,
			TEXT("Prototype_GameObject_InventoryStoreSlot"),
			CUI_Manager::PERSISTENT, &InventoryStoreSlotDesc);

		if (nullptr == pInventoryStoreSlot) return E_FAIL;
		pInventoryStoreSlot->Set_Parent(this);  // 부모 설정

		m_UIStoreSlots[i] = dynamic_cast<CInventoryStoreSlot*>(pInventoryStoreSlot);
		m_UIStoreSlots[i]->Set_Slot_Index(i);
	}

	_float  fLeftSlotsWidth = 80.f; // 슬롯의 너비

	// 근접 무기 슬롯
	CInventorySlot::INVENTORY_SLOT_DESC     InventoryMeleeSlot
	(TEXT("GameObject_InventoryMeleeSlot"), CUIObject::UNCLICKABLE,
		92.f, 202.2f, 0.4f, fLeftSlotsWidth, fLeftSlotsWidth,
		L"Prototype_Component_Texture_InventoryGearSlot");

	CUIObject* pInventoryGearSlot = m_pGameInstance->Add_UIObject(LEVEL_STATIC, LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_InventoryGearSlot"),
		CUI_Manager::PERSISTENT, &InventoryMeleeSlot);

	if (nullptr == pInventoryGearSlot) return E_FAIL;
	pInventoryGearSlot->Set_Parent(this);  // 부모 설정
	m_UIGearSlots[0] = dynamic_cast<CInventoryGearSlot*>(pInventoryGearSlot);
	m_UIGearSlots[0]->Set_Slot_Index(0);

	// 갑옷 슬롯
	CInventorySlot::INVENTORY_SLOT_DESC     InventoryArmorSlotDesc
	(TEXT("GameObject_InventoryArmorSlot"), CUIObject::UNCLICKABLE,
		237.7f, 174.7f, 0.4f, fLeftSlotsWidth, fLeftSlotsWidth,
		L"Prototype_Component_Texture_InventoryGearSlot");

	pInventoryGearSlot = m_pGameInstance->Add_UIObject(LEVEL_STATIC, LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_InventoryGearSlot"),
		CUI_Manager::PERSISTENT, &InventoryArmorSlotDesc);

	if (nullptr == pInventoryGearSlot) return E_FAIL;
	pInventoryGearSlot->Set_Parent(this);  // 부모 설정
	m_UIGearSlots[1] = dynamic_cast<CInventoryGearSlot*>(pInventoryGearSlot);
	m_UIGearSlots[1]->Set_Slot_Index(1);


	// 원거리 무기 슬롯
	CInventorySlot::INVENTORY_SLOT_DESC     InventoryRangedSlotDesc
	(TEXT("GameObject_InventoryRangedSlot"), CUIObject::UNCLICKABLE,
		383.5f, 202.2f, 0.4f, fLeftSlotsWidth, fLeftSlotsWidth,
		L"Prototype_Component_Texture_InventoryGearSlot");

	pInventoryGearSlot = m_pGameInstance->Add_UIObject(LEVEL_STATIC, LEVEL_GAMEPLAY,
		TEXT("Prototype_GameObject_InventoryGearSlot"),
		CUI_Manager::PERSISTENT, &InventoryRangedSlotDesc);

	if (nullptr == pInventoryGearSlot) return E_FAIL;
	pInventoryGearSlot->Set_Parent(this);  // 부모 설정
	m_UIGearSlots[2] = dynamic_cast<CInventoryGearSlot*>(pInventoryGearSlot);
	m_UIGearSlots[2]->Set_Slot_Index(2);


	// 유물 슬롯
	for (int i = 0; i < 3; ++i)
	{
		_float  fItemSlotStartX = 126.5f;   // 첫 번째 슬롯의 시작 위치

		_float  fItemSlotSpacing = 31.5f;  // 슬롯 간 간격

		_float  fSlotX = fItemSlotStartX + static_cast<float>(i) * (fLeftSlotsWidth + fItemSlotSpacing); // 겹치지 않도록 계산

		const _wstring& strSlotName = L"GameObject_InventoryArtifactSlot_Empty_" + std::to_wstring(i);

		CInventorySlot::INVENTORY_SLOT_DESC     InventoryArtifactSlotDesc
		(strSlotName.c_str(), CUIObject::UNCLICKABLE,
			fSlotX, 626.f, 0.4f, 80.f, 80.f,
			L"Prototype_Component_Texture_InventoryArtifactSlot_Empty");

		CUIObject* pInventoryArtifactSlot = m_pGameInstance->Add_UIObject(LEVEL_STATIC, LEVEL_GAMEPLAY,
			TEXT("Prototype_GameObject_InventoryArtifactSlot_Empty"),
			CUI_Manager::PERSISTENT, &InventoryArtifactSlotDesc);

		if (nullptr == pInventoryArtifactSlot)  return E_FAIL;
		pInventoryArtifactSlot->Set_Parent(this);  // 부모 설정
		m_UIArtifactSlots[i] = dynamic_cast<CInventoryArtifactSlot*>(pInventoryArtifactSlot);
		m_UIArtifactSlots[i]->Set_Slot_Index(i);
	}

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

void CInventoryBase::Add_Icon_To_StoreSlot(const Item_Added_To_StoreSlot& event)
{
	if (false == m_bReadyForEvents)
		return;

	if (false == m_UIStoreSlots[event.iStoreSlotIndex]->Is_Empty() ||
		event.iStoreSlotIndex < 0 ||
		event.iStoreSlotIndex >= m_UIStoreSlots.size())
		return;

	CInventoryStoreSlot*  pInventoryStoreSlot = m_UIStoreSlots[event.iStoreSlotIndex];
	CItem* pItem = event.pItem;

	pInventoryStoreSlot->Add_Icon_Image(pItem->Get_IconGameObjectTag(), pItem->Get_IconTexPrototypeTag(), pItem->Get_ItemType());
	pInventoryStoreSlot->Set_Empty(false);
}

void CInventoryBase::Unequip_Icon_To_StoreSlot(const Unequipped_To_StoreSlot& event)
{
	if (false == m_UIStoreSlots[event.iStoreSlotIndex]->Is_Empty() ||
		event.iStoreSlotIndex < 0 || event.iStoreSlotIndex >= static_cast<_int>(m_UIStoreSlots.size()) ||
		event.iOtherSlotIndex < 0 || event.iOtherSlotIndex >= static_cast<_int>(m_UIGearSlots.size()))
		return;

	switch (event.pItem->Get_ItemType())
	{
	case ITEM_TYPE::MELEE:
	case ITEM_TYPE::ARMOR:
	case ITEM_TYPE::RANGED:
	{
		CInventoryGearSlot* pInventoryGearSlot = m_UIGearSlots[event.iOtherSlotIndex];

		if (pInventoryGearSlot->Is_Empty())
			return;

		if (FAILED(pInventoryGearSlot->Clear_Icon()))
			return;
	}
	break;

	case ITEM_TYPE::ARTIFACT:
	{
		CInventoryArtifactSlot* pInventoryArtifactSlot = m_UIArtifactSlots[event.iOtherSlotIndex];

		if (pInventoryArtifactSlot->Is_Empty())
			return;

		if (FAILED(pInventoryArtifactSlot->Clear_Icon()))
			return;
	}
	break;
	}

	CInventoryStoreSlot* pInventoryStoreSlot = m_UIStoreSlots[event.iStoreSlotIndex];
	CItem* pItem = event.pItem;

	pInventoryStoreSlot->Add_Icon_Image(pItem->Get_IconGameObjectTag(), pItem->Get_IconTexPrototypeTag(), pItem->Get_ItemType());
	pInventoryStoreSlot->Set_Empty(false);
}

void CInventoryBase::Swap_Store_and_Gear_Icon(const Swap_Store_with_Gear& swap_Store_GearEvent)
{
	if (swap_Store_GearEvent.iStoreSlotIndex < 0 || swap_Store_GearEvent.iStoreSlotIndex > m_UIStoreSlots.size() ||
		swap_Store_GearEvent.iGearSlotIndex < 0 || swap_Store_GearEvent.iGearSlotIndex > m_UIGearSlots.size())
		return;

	CInventoryStoreSlot* pInventoryStoreSlot = m_UIStoreSlots[swap_Store_GearEvent.iStoreSlotIndex];
	CInventoryGearSlot*  pInventoryGearSlot  = m_UIGearSlots[swap_Store_GearEvent.iGearSlotIndex];
	if (FAILED(pInventoryStoreSlot->Clear_Icon()) || FAILED(pInventoryGearSlot->Clear_Icon()))
		return;

	CItem* pStoreItem = swap_Store_GearEvent.pSwappedStoreItem;
	CItem* pGearItem  = swap_Store_GearEvent.pSwappedGearItem;

	pInventoryStoreSlot->Add_Icon_Image(pStoreItem->Get_IconGameObjectTag(), pStoreItem->Get_IconTexPrototypeTag(), pStoreItem->Get_ItemType());
	pInventoryGearSlot->Add_Icon_Image(pGearItem->Get_IconGameObjectTag(), pGearItem->Get_IconTexPrototypeTag(), pGearItem->Get_ItemType());
}

void CInventoryBase::Swap_Store_and_Artifact_Icon(const Swap_Store_with_Artifact& swap_Store_ArtifactEvent)
{
	if (swap_Store_ArtifactEvent.iStoreSlotIndex < 0 || swap_Store_ArtifactEvent.iStoreSlotIndex > m_UIStoreSlots.size() ||
		swap_Store_ArtifactEvent.iArtifactSlotIndex < 0 || swap_Store_ArtifactEvent.iArtifactSlotIndex > m_UIGearSlots.size())
		return;

	CInventoryStoreSlot* pInventoryStoreSlot = m_UIStoreSlots[swap_Store_ArtifactEvent.iStoreSlotIndex];
	CInventoryArtifactSlot* pInventoryArtifactSlot = m_UIArtifactSlots[swap_Store_ArtifactEvent.iArtifactSlotIndex];
	if (FAILED(pInventoryStoreSlot->Clear_Icon()) || FAILED(pInventoryArtifactSlot->Clear_Icon()))
		return;

	CItem* pStoreItem = swap_Store_ArtifactEvent.pSwappedStoreItem;
	CItem* pArtifactItem = swap_Store_ArtifactEvent.pSwappedArtifactItem;

	pInventoryStoreSlot->Add_Icon_Image(pStoreItem->Get_IconGameObjectTag(), pStoreItem->Get_IconTexPrototypeTag(), pStoreItem->Get_ItemType());
	pInventoryArtifactSlot->Add_Icon_Image(pArtifactItem->Get_IconGameObjectTag(), pArtifactItem->Get_IconTexPrototypeTag(), pArtifactItem->Get_ItemType());
}

void CInventoryBase::Equip_Icon_To_GearSlot(const Equip_To_Gear& equip_To_GearEvent)
{
	if (equip_To_GearEvent.iStoreSlotIndex < 0 || equip_To_GearEvent.iStoreSlotIndex > m_UIStoreSlots.size() ||
		equip_To_GearEvent.iGearSlotIndex < 0 || equip_To_GearEvent.iGearSlotIndex > m_UIGearSlots.size())
		return;

	CInventoryStoreSlot* pInventoryStoreSlot = m_UIStoreSlots[equip_To_GearEvent.iStoreSlotIndex];
	CInventoryGearSlot*  pInventoryGearSlot  = m_UIGearSlots[equip_To_GearEvent.iGearSlotIndex];
	if (FAILED(pInventoryStoreSlot->Clear_Icon()))  return;
	pInventoryStoreSlot->Set_Empty(true);

	CItem* pGearItem = equip_To_GearEvent.pEquippedItem;

	pInventoryGearSlot->Add_Icon_Image(pGearItem->Get_IconGameObjectTag(), pGearItem->Get_IconTexPrototypeTag(), pGearItem->Get_ItemType());
	pInventoryGearSlot->Set_Empty(false);
}

void CInventoryBase::Equip_Icon_To_ArtifactSlot(const Equip_To_Artifact& equip_To_ArtifactEvent)
{
	if (equip_To_ArtifactEvent.iStoreSlotIndex < 0 || equip_To_ArtifactEvent.iStoreSlotIndex > m_UIStoreSlots.size() ||
		equip_To_ArtifactEvent.iArtifactSlotIndex < 0 || equip_To_ArtifactEvent.iArtifactSlotIndex > m_UIGearSlots.size())
		return;

	CInventoryStoreSlot* pInventoryStoreSlot = m_UIStoreSlots[equip_To_ArtifactEvent.iStoreSlotIndex];
	CInventoryArtifactSlot* pInventoryArtifactSlot = m_UIArtifactSlots[equip_To_ArtifactEvent.iArtifactSlotIndex];
	if (FAILED(pInventoryStoreSlot->Clear_Icon()))	return;
	pInventoryStoreSlot->Set_Empty(true);

	CItem* pGearItem = equip_To_ArtifactEvent.pEquippedItem;

	pInventoryArtifactSlot->Add_Icon_Image(pGearItem->Get_IconGameObjectTag(), pGearItem->Get_IconTexPrototypeTag(), pGearItem->Get_ItemType());
	pInventoryArtifactSlot->Set_Empty(false);
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

	/*for (auto& storeSlot : m_UIStoreSlots)
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
	m_UIArtifactSlots.clear();*/

	Safe_Delete(m_pDesc);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}