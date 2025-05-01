#include "InventorySlot.h"
#include "InventoryIcon.h"
#include "GameInstance.h"

_int CInventorySlot::m_iIconGameObjectTagID = 0;

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

HRESULT CInventorySlot::Add_Icon_Image(const _wstring& strGameObjectTag, const _wstring& strIconTexPrototypeTag, ITEM_TYPE eItemType)
{
    // GameObjectTag에 고유한 ID를 추가
    _wstring    uniqueTag = strGameObjectTag + std::to_wstring(m_iIconGameObjectTagID++);

    CInventoryIcon::INVENTORY_ICON_DESC   InventoryIconDesc(uniqueTag,
															CUIObject::CLICKABLE,
                                                            m_fX, m_fY, m_fZ - 0.1f,
                                                            m_fSizeX * 0.8f, m_fSizeY * 0.8f,
                                                            strIconTexPrototypeTag, eItemType);

    CUIObject* pIconObject = m_pGameInstance->Add_UIObject(LEVEL_STATIC, LEVEL_GAMEPLAY,
														TEXT("Prototype_GameObject_InventoryIcon"),
														CUI_Manager::PERSISTENT, &InventoryIconDesc);

    if (nullptr == pIconObject)
        return E_FAIL;

    pIconObject->Set_Parent(this);
    m_pIcon = dynamic_cast<CInventoryIcon*>(pIconObject);

    return S_OK;
}


HRESULT CInventorySlot::Clear_Icon()
{
    const _wstring& gameObjectTag = m_pIcon->Get_Icon_Desc()->strGameObjectTag;

    m_pGameInstance->Request_Delete_UIObject(gameObjectTag, CUI_Manager::PERSISTENT);

    return S_OK;
}

void CInventorySlot::Free()
{
    __super::Free();

    //Safe_Release(m_pIcon);

    Safe_Delete(m_pDesc);
    Safe_Release(m_pEmptyTextureCom);
    Safe_Release(m_pSlotTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
