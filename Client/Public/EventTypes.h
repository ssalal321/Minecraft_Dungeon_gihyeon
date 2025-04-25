#pragma once

#pragma once
namespace Client
{
    class CItem;

    struct Item_Added
	{
        CItem*  pItem = { nullptr };
        int     slotIndex = { -1 };

        std::wstring    pGameObjectTag;
        std::wstring	m_strIconTexPrototypeTag = {};

    };

   /* struct Add_Icon
    {
        float		    fZ, fSizeX, fSizeY;

        std::wstring    pGameObjectTag;
        std::wstring	m_strIconTexPrototypeTag = {};
    };*/

}
