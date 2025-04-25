#include "InventorySlot.h"
#include "InventoryIcon.h"
#include "GameInstance.h"

CInventorySlot::CInventorySlot(ID3D11Device* device, ID3D11DeviceContext* context)
    : CUIObject(device, context) {
}

CInventorySlot::~CInventorySlot()
{
}

HRESULT CInventorySlot::Initialize(void* pArg)
{
    __super::Initialize(pArg);
    
    return S_OK;
}

//void CInventorySlot::Create_Icon(const wstring& texPrototypeTag)
//{
//    m_pIcon = new CInventoryIcon(m_pDevice, m_pContext);
//
//    CInventoryIcon::UIOBJECT_DESC iconDesc = {
//        L"InventoryIcon",
//        UI_STATE::CLICKABLE,
//        0.f, 0.f, 0.f,
//        m_fSizeX, m_fSizeY,
//        texPrototypeTag
//    };
//
//    m_pIcon->Initialize(&iconDesc);
//    m_pIcon->Set_Parent(this);
//}
//
//void CInventorySlot::Set_Item(const ItemData* pItem) {
//    m_pItemData = pItem;
//
//    if (m_pIcon)
//        m_pIcon->Set_Item(pItem);
//}
//
//void CInventorySlot::Clear_Item() {
//    m_pItemData = nullptr;
//
//    if (m_pIcon)
//        m_pIcon->Clear();
//}


void CInventorySlot::Free()
{
    __super::Free();

    Safe_Release(m_pIcon);

    Safe_Delete(m_pDesc);
    Safe_Release(m_pEmptyTextureCom);
    Safe_Release(m_pSlotTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}