#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class  CGameInstance;
END

BEGIN(Client)
class  CInventoryStoreSlot;
class  CInventoryGearSlot;
class  CInventoryArtifactSlot;
class  CItem;

class CInventoryData : public CBase
{
private:
	CInventoryData();
	~CInventoryData()	override = default;

public:
	HRESULT		Initialize();

	void		Add_Item_To_StoreSlot(Client::CItem* pItem);

	void		Equip_From_StoreSlot(_int storeSlotIndex);
	void		Unequip_Gear(GEAR_TYPE gearType);
	void		Unequip_Artifact(_int artifactSlotIndex);

private:
	CGameInstance*		m_pGameInstance = { nullptr };

	vector<CItem*>		m_StoreSlots;
	vector<CItem*>		m_GearSlots;       // 고정 크기 3
	vector<CItem*>		m_ArtifactSlots;   // 고정 크기 3

	CItem*				m_pItem = { nullptr };
	ITEMTYPE			m_eWeaponType = {ITEMTYPE::NONE };

	//_int		m_iSlotIndex = { -1 };

private:
	_int		Find_Empty_ArtifactSlot();
	_int		Find_Empty_StoreSlot();

	void		Swap_With_Gear(_int iStoreSlotIndex, _int iGearSlotIndex);
	void		Swap_With_Artifact(_int iStoreSlotIndex, _int iItemSlotIndex);

public:
	static	 CInventoryData*	Create();
	void	 Free()		override;
};

END

