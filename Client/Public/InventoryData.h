#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class  CGameInstance;
END

BEGIN(Client)
	class CPlayer;
	class  CInventoryStoreSlot;
class  CInventoryGearSlot;
class  CInventoryArtifactSlot;

class CInventoryData : public CBase
{
private:
	CInventoryData();
	~CInventoryData()	override = default;

public:
	HRESULT		Initialize(CPlayer* pPlayer);

	void		Add_Item_To_StoreSlot(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strItemObjectTag, void* pItemDesc);

	void		Equip_From_StoreSlot(const DoubleClicked_From_StoreSlot& event);
	void		Unequip_GearSlot(const DoubleClicked_From_GearSlot& event);
	void		Unequip_ArtifactSlot(const DoubleClicked_From_ArtifactSlot& event);

private:
	CGameInstance*		m_pGameInstance = { nullptr };

	vector<CItem*>		m_StoreSlots;
	vector<CItem*>		m_GearSlots;       // 고정 크기 3
	vector<CItem*>		m_ArtifactSlots;   // 고정 크기 3

	CItem*				m_pItem = { nullptr };
	ITEM_TYPE			m_eWeaponType = {ITEM_TYPE::NONE };

	CPlayer*			m_pPlayer = { nullptr };

	//_int		m_iSlotIndex = { -1 };

private:
	_int		Find_Empty_ArtifactSlot();
	_int		Find_Empty_StoreSlot();

	HRESULT		Swap_With_Gear(_int iStoreSlotIndex, _int iGearSlotIndex);
	HRESULT		Swap_With_Artifact(_int iStoreSlotIndex, _int iItemSlotIndex);

public:
	static	 CInventoryData*	Create(CPlayer* pPlayer);
	void	 Free()		override;
};

END

