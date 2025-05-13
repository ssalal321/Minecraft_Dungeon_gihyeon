#include "InventoryData.h"
#include "GameInstance.h"
#include "Item.h"
#include "Player.h"

CInventoryData::CInventoryData()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CInventoryData::Initialize(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;

	m_StoreSlots.resize(STORESLOTSIZE, nullptr);
	m_GearSlots.resize(GEARSLOTSIZE, nullptr);
	m_ArtifactSlots.resize(ARTIFACTSLOTSIZE, nullptr);

	m_pGameInstance->Subscribe<DoubleClicked_From_StoreSlot>(
		[this](const DoubleClicked_From_StoreSlot& doubleClicked_From_StoreEvent) { this->Equip_From_StoreSlot(doubleClicked_From_StoreEvent); }
	);

	m_pGameInstance->Subscribe<DoubleClicked_From_GearSlot>(
		[this](const DoubleClicked_From_GearSlot& doubleClicked_From_GearEvent) { this->Unequip_GearSlot(doubleClicked_From_GearEvent); }
	);

	m_pGameInstance->Subscribe<DoubleClicked_From_ArtifactSlot>(
		[this](const DoubleClicked_From_ArtifactSlot& doubleClicked_From_ArtifactEvent) { this->Unequip_ArtifactSlot(doubleClicked_From_ArtifactEvent); }
	);

	return S_OK;
}

void CInventoryData::Add_Item_To_StoreSlot(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag,
										   const _wstring& strItemObjectTag, void* pItemDesc)
{
	for (const auto& pExistingItem : m_StoreSlots)  // 이미 있는 Item인지 검사
	{
		if (pExistingItem && pExistingItem->Get_GameObjectTag() == strItemObjectTag)
			return;
	}

	CItem* pItem = dynamic_cast<CItem*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTOTYPE_GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, pItemDesc));

	for (size_t i = 0; i < m_StoreSlots.size(); ++i)
	{
		if (nullptr == m_StoreSlots[i])
		{
			m_StoreSlots[i] = pItem;

			Item_Added_To_StoreSlot itemAddedEvent{ static_cast<_int>(i), m_StoreSlots[i] };
			m_pGameInstance->Publish(itemAddedEvent);

			break;
		}
	}
}


void CInventoryData::Equip_From_StoreSlot(const DoubleClicked_From_StoreSlot& event)  // 몇 번째 StoreSlot에서 아이템 가져올 것인가?
{
	if (event.iStoreSlotIndex < 0 || event.iStoreSlotIndex >= static_cast<_int>(m_StoreSlots.size()) ||
		nullptr == m_StoreSlots[event.iStoreSlotIndex])
		return;

	CItem* pStoreItem = m_StoreSlots[event.iStoreSlotIndex];

	switch (pStoreItem->Get_ItemType())
	{
	case ITEM_TYPE::MELEE:
	case ITEM_TYPE::ARMOR:
	case ITEM_TYPE::RANGED:
	{
		// Gear 슬롯 인덱스 매칭
		_int gearIndex = 0;
		if (pStoreItem->Get_ItemType() == ITEM_TYPE::ARMOR)
			gearIndex = 1;
		else if (pStoreItem->Get_ItemType() == ITEM_TYPE::RANGED)
			gearIndex = 2;


		CItem*  pGearItem = m_GearSlots[gearIndex];

		if (nullptr != pGearItem)  // 빈 슬롯이 없으면
		{
			if (FAILED(Swap_With_Gear(event.iStoreSlotIndex, gearIndex)))
				return;

			Swap_Store_with_Gear  swapStoreWithGearEvent{ event.iStoreSlotIndex, gearIndex, m_StoreSlots[event.iStoreSlotIndex], m_GearSlots[gearIndex]};
			m_pGameInstance->Publish(swapStoreWithGearEvent);
		}
		else
		{
			m_GearSlots[gearIndex] = pStoreItem;
			m_StoreSlots[event.iStoreSlotIndex] = nullptr;
			if (FAILED(m_pPlayer->Add_PartObject(pStoreItem, pStoreItem->Get_PartObjectTag())))
				return;

			Equip_To_Gear  equipToGearEvent{ event.iStoreSlotIndex, gearIndex, m_GearSlots[gearIndex] };
			m_pGameInstance->Publish(equipToGearEvent);
		}
		break;
	}

	case ITEM_TYPE::ARTIFACT:
	{
		_int emptyIndex = Find_Empty_ArtifactSlot();

		if (emptyIndex == -1)  // 빈 슬롯이 없으면
		{
			if (FAILED(Swap_With_Artifact(event.iStoreSlotIndex, 0)))
				return;

			Swap_Store_with_Artifact  swapStoreWithArtifactEvent{ event.iStoreSlotIndex, 0, m_StoreSlots[event.iStoreSlotIndex], m_ArtifactSlots[0] };
			m_pGameInstance->Publish(swapStoreWithArtifactEvent);
		}
		else
		{
			m_ArtifactSlots[emptyIndex] = pStoreItem;
			m_StoreSlots[event.iStoreSlotIndex] = nullptr;
			if (FAILED(m_pPlayer->Add_PartObject(pStoreItem, pStoreItem->Get_PartObjectTag())))
				return;

			Equip_To_Artifact  equipToArtifactEvent{ event.iStoreSlotIndex, emptyIndex, m_ArtifactSlots[emptyIndex] };
			m_pGameInstance->Publish(equipToArtifactEvent);
		}
		break;
	}

	default:
		break;
	}
}

void CInventoryData::Unequip_GearSlot(const DoubleClicked_From_GearSlot& event)
{
	if (event.iGearSlotIndex < 0 || event.iGearSlotIndex >= GEARSLOTSIZE ||
		nullptr == m_GearSlots[event.iGearSlotIndex])
		return;

	_int emptyStoreIndex = Find_Empty_StoreSlot();
	if (emptyStoreIndex == -1)
		return;

	m_StoreSlots[emptyStoreIndex] = m_GearSlots[event.iGearSlotIndex];
	m_GearSlots[event.iGearSlotIndex] = nullptr;

	CItem* pSwappedStoreItem = m_StoreSlots[emptyStoreIndex];
	if (FAILED(m_pPlayer->Delete_PartObject(pSwappedStoreItem->Get_PartObjectTag(), false)))
		return;

	Unequipped_To_StoreSlot  unequippedToStoreEvent{ emptyStoreIndex, event.iGearSlotIndex, m_StoreSlots[emptyStoreIndex] };
	m_pGameInstance->Publish(unequippedToStoreEvent);
}

void CInventoryData::Unequip_ArtifactSlot(const DoubleClicked_From_ArtifactSlot& event)
{
	if (event.iArtifactSlotIndex < 0 || event.iArtifactSlotIndex >= ARTIFACTSLOTSIZE ||
		nullptr == m_ArtifactSlots[event.iArtifactSlotIndex])
		return;

	_int emptyStoreIndex = Find_Empty_StoreSlot();
	if (emptyStoreIndex == -1) 
		return;

	m_StoreSlots[emptyStoreIndex] = m_ArtifactSlots[event.iArtifactSlotIndex];
	m_ArtifactSlots[event.iArtifactSlotIndex] = nullptr;

	CItem* pSwappedStoreItem = m_StoreSlots[emptyStoreIndex];
	if (FAILED(m_pPlayer->Delete_PartObject(pSwappedStoreItem->Get_PartObjectTag(), false)))
		return;

	Unequipped_To_StoreSlot  unequippedToStoreEvent{ emptyStoreIndex, event.iArtifactSlotIndex, m_StoreSlots[emptyStoreIndex] };
	m_pGameInstance->Publish(unequippedToStoreEvent);
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

HRESULT CInventoryData::Swap_With_Gear(_int iStoreSlotIndex, _int iGearSlotIndex)
{
	if (iStoreSlotIndex < 0 || iStoreSlotIndex >= static_cast<_int>(m_StoreSlots.size())) return E_FAIL;
	if (iGearSlotIndex < 0 || iGearSlotIndex >= static_cast<_int>(m_GearSlots.size())) return E_FAIL;

	if (!m_StoreSlots[iStoreSlotIndex] || !m_GearSlots[iGearSlotIndex])
		return E_FAIL;

	std::swap(m_StoreSlots[iStoreSlotIndex], m_GearSlots[iGearSlotIndex]);

	CItem* pSwappedStoreItem = m_StoreSlots[iStoreSlotIndex];
	CItem* pSwappedGear = m_GearSlots[iGearSlotIndex];

	if (FAILED(m_pPlayer->Delete_PartObject(pSwappedStoreItem->Get_PartObjectTag(), false)))
		return E_FAIL;

	if (FAILED(m_pPlayer->Add_PartObject(pSwappedGear, pSwappedGear->Get_PartObjectTag())))
		return E_FAIL;

	return S_OK;
}

HRESULT CInventoryData::Swap_With_Artifact(_int iStoreSlotIndex, _int iItemSlotIndex)
{
	if (iStoreSlotIndex < 0 || iStoreSlotIndex >= static_cast<_int>(m_StoreSlots.size())) return E_FAIL;
	if (iItemSlotIndex < 0 || iItemSlotIndex >= static_cast<_int>(m_ArtifactSlots.size())) return E_FAIL;

	if (!m_StoreSlots[iStoreSlotIndex] || !m_ArtifactSlots[iItemSlotIndex])
		return E_FAIL;

	std::swap(m_StoreSlots[iStoreSlotIndex], m_ArtifactSlots[iItemSlotIndex]);

	CItem* pSwappedStoreItem = m_StoreSlots[iStoreSlotIndex];
	CItem* pSwappedArtifact = m_ArtifactSlots[iItemSlotIndex];

	if (FAILED(m_pPlayer->Delete_PartObject(pSwappedStoreItem->Get_PartObjectTag(), false)))
		return E_FAIL;

	if (FAILED(m_pPlayer->Add_PartObject(pSwappedArtifact, pSwappedArtifact->Get_PartObjectTag())))
		return E_FAIL;

	return S_OK;
}

CInventoryData* CInventoryData::Create(CPlayer* pPlayer)
{
	CInventoryData* pGameInstance = new CInventoryData();

	if (FAILED(pGameInstance->Initialize(pPlayer)))
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
