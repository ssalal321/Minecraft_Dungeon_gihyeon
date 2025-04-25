#include "InventoryData.h"
#include "GameInstance.h"
#include "Item.h"

CInventoryData::CInventoryData()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CInventoryData::Initialize()
{
	m_StoreSlots.resize(STORESLOTSIZE, nullptr);
	m_GearSlots.resize(GEARSLOTSIZE, nullptr);
	m_ArtifactSlots.resize(ARTIFACTSLOTSIZE, nullptr);

	return S_OK;
}

void CInventoryData::Add_Item_To_StoreSlot(CItem* pItem)
{
	for (size_t i = 0; i < m_StoreSlots.size(); ++i)
	{
		if (nullptr == m_StoreSlots[i])
		{
			m_StoreSlots[i] = pItem;

			Item_Added itemAddedEvent{ pItem, static_cast<_int>(i), pItem->Get_IconGameObjectTag(), pItem->Get_TexPrototypeTag() };
			m_pGameInstance->Publish(itemAddedEvent);

			break;
		}
	}
}

void CInventoryData::Equip_From_StoreSlot(_int storeSlotIndex)  // 몇 번째 StoreSlot에서 아이템 가져올 것인가?
{
	if (storeSlotIndex < 0 || storeSlotIndex >= static_cast<_int>(m_StoreSlots.size()) ||
		nullptr == m_StoreSlots[storeSlotIndex])
		return;

	CItem* pItem = m_StoreSlots[storeSlotIndex];

	switch (pItem->Get_ItemType())
	{
	case ITEMTYPE::MELEE:
	case ITEMTYPE::ARMOR:
	case ITEMTYPE::RANGED:
	{
		// Gear 슬롯 인덱스 매칭
		_int gearIndex = 0;
		if (pItem->Get_ItemType() == ITEMTYPE::ARMOR)
			gearIndex = 1;
		else if (pItem->Get_ItemType() == ITEMTYPE::RANGED)
			gearIndex = 2;

		CItem*  pGearItem = m_GearSlots[gearIndex];

		if (nullptr != pGearItem)
		{
			Swap_With_Gear(storeSlotIndex, gearIndex);
		}
		else
		{
			m_GearSlots[gearIndex] = pItem;
			m_StoreSlots[storeSlotIndex] = nullptr;
		}

		//Update_SlotTexture(pGearSlot, pItem->Get_TexPrototypeTag());
		break;
	}

	case ITEMTYPE::ARTIFACT:
	{
		_int emptyIndex = Find_Empty_ArtifactSlot();

		if (emptyIndex == -1)
		{
			CItem*  pArtfifactItem = m_ArtifactSlots[0];

			Swap_With_Artifact(storeSlotIndex, 0);

			//Update_SlotTexture(pArtifactSlot, pItem->Get_TexPrototypeTag());
		}
		else
		{
			m_ArtifactSlots[emptyIndex] = pItem;
			m_StoreSlots[storeSlotIndex] = nullptr;

			//Update_SlotTexture(pArtifactSlot, pItem->Get_TexPrototypeTag());
		}
		break;
	}

	default:
		break;
	}
}

void CInventoryData::Unequip_Gear(GEAR_TYPE gearType)
{
	_int gearTypeIndex = static_cast<_int>(gearType);
	if (gearTypeIndex < 0 || gearTypeIndex >= GEARSLOTSIZE ||
		nullptr == m_GearSlots[gearTypeIndex])
		return;

	_int emptyStoreIndex = Find_Empty_StoreSlot();
	if (emptyStoreIndex == -1)
		return;

	m_StoreSlots[emptyStoreIndex] = m_GearSlots[gearTypeIndex];
	m_GearSlots[gearTypeIndex] = nullptr;

	//Update_SlotTexture(pStoreSlot, pStoreSlot->Get_Item()->Get_TexPrototypeTag());
	//Update_SlotTexture(pGearSlot, TEXT("Empty"));
}

void CInventoryData::Unequip_Artifact(_int artifactSlotIndex)
{
	if (artifactSlotIndex < 0 || artifactSlotIndex >= ARTIFACTSLOTSIZE ||
		nullptr == m_ArtifactSlots[artifactSlotIndex])
		return;

	_int emptyStoreIndex = Find_Empty_StoreSlot();
	if (emptyStoreIndex == -1) 
		return;

	m_StoreSlots[emptyStoreIndex] = m_ArtifactSlots[artifactSlotIndex];
	m_ArtifactSlots[artifactSlotIndex] = nullptr;

	//Update_SlotTexture(pStoreSlot, pStoreSlot->Get_Item()->Get_TexPrototypeTag());
	//Update_SlotTexture(pItemSlot, TEXT("Empty"));
}

_int CInventoryData::Find_Empty_StoreSlot()
{
	for (size_t i = 0; i < m_StoreSlots.size(); ++i)
	{
		if (nullptr == m_StoreSlots[static_cast<_int>(i)])
			return static_cast<_int>(i);
	}
	return -1;
}

_int CInventoryData::Find_Empty_ArtifactSlot()
{
	for (size_t i = 0; i < m_ArtifactSlots.size(); ++i)
	{
		if (nullptr == m_ArtifactSlots[static_cast<_int>(i)])
			return static_cast<_int>(i);
	}
	return -1;
}

void CInventoryData::Swap_With_Gear(_int iStoreSlotIndex, _int iGearSlotIndex)
{
	if (iStoreSlotIndex < 0 || iStoreSlotIndex >= static_cast<_int>(m_StoreSlots.size())) return;
	if (iGearSlotIndex < 0 || iGearSlotIndex >= static_cast<_int>(m_GearSlots.size())) return;

	CItem* pStoreItem  = m_StoreSlots[iStoreSlotIndex];
	CItem* pGearItem   = m_GearSlots[iGearSlotIndex];

	if (!pStoreItem || !pGearItem)
		return;

	std::swap(pStoreItem, pGearItem);

	// Icon 처리도 해줘야
}

void CInventoryData::Swap_With_Artifact(_int iStoreSlotIndex, _int iItemSlotIndex)
{
	if (iStoreSlotIndex < 0 || iStoreSlotIndex >= m_StoreSlots.size()) return;
	if (iItemSlotIndex < 0 || iItemSlotIndex >= m_ArtifactSlots.size()) return;

	CItem* pStoreItem	  = m_StoreSlots[iStoreSlotIndex];
	CItem* pArtifactItem  = m_ArtifactSlots[iItemSlotIndex];

	if (!pStoreItem || !pArtifactItem) return;

	std::swap(pStoreItem, pArtifactItem);

	// Icon 처리도 해줘야
}

CInventoryData* CInventoryData::Create()
{
	CInventoryData* pGameInstance = new CInventoryData();

	if (FAILED(pGameInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CInventoryData");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CInventoryData::Free()
{
	__super::Free();

	for (auto& storeSlot : m_StoreSlots)
	{
		Safe_Release(storeSlot);
	}
	m_StoreSlots.clear();

	for (auto& gearSlot : m_GearSlots)
	{
		Safe_Release(gearSlot);
	}
	m_GearSlots.clear();

	for (auto& artifactSlot : m_ArtifactSlots)
	{
		Safe_Release(artifactSlot);
	}
	m_ArtifactSlots.clear();

	Safe_Release(m_pGameInstance);
}
