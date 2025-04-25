#pragma once

#pragma once
namespace Client
{
    class CItem;

    struct Item_Added
	{
        CItem*  pItem = { nullptr };
        int     slotIndex = { -1 };
        std::wstring	m_strIconTexPrototypeTag = {};
    };

}
