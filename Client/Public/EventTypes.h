#pragma once

#pragma once
namespace Client
{
    class CItem;

    struct Item_Added_To_StoreSlot
	{
        int     iStoreSlotIndex = { -1 };

        CItem*  pItem = { nullptr };
    };

    struct Unequipped_To_StoreSlot
    {
        int     iStoreSlotIndex = { -1 };
        int     iGearSlotIndex = { -1 };

        CItem* pItem = { nullptr };
    };

    struct DoubleClicked_From_StoreSlot
    {
        int    iStoreSlotIndex = { -1 };
    };

    struct DoubleClicked_From_GearSlot
    {
        int    iGearSlotIndex = { -1 };
    };

    struct DoubleClicked_From_ArtifactSlot
    {
        int    iArtifactSlotIndex = { -1 };
    };

    struct Swap_Store_with_Gear
    {
	    int     iStoreSlotIndex = { -1 };
        int     iGearSlotIndex  = { -1 };

        CItem*  pSwappedStoreItem = { nullptr };
        CItem*  pSwappedGearItem  = { nullptr };
    };

    struct Equip_To_Gear
    {
        int     iStoreSlotIndex = { -1 };
        int     iGearSlotIndex = { -1 };

        CItem*  pEquippedItem = { nullptr };
    };

    struct Swap_Store_with_Artifact
    {
        int     iStoreSlotIndex = { -1 };
        int     iArtifactSlotIndex = { -1 };

        CItem*  pSwappedStoreItem = { nullptr };
        CItem*  pSwappedArtifactItem = { nullptr };
    };

    struct Equip_To_Artifact
    {
        int     iStoreSlotIndex = { -1 };
        int     iArtifactSlotIndex = { -1 };

        CItem*  pEquippedItem = { nullptr };
    };

}
