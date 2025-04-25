#include "InventoryIcon.h"
#include "GameInstance.h"

CInventoryIcon::CInventoryIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject(pDevice, pContext)
{
}

void CInventoryIcon::Set_Item()
{
	Set_Visible(true);
	// 텍스처 변경 등
}

void CInventoryIcon::Clear()
{
    Set_Visible(false);
}

CInventoryIcon* CInventoryIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInventoryIcon* pGameInstance = new CInventoryIcon(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CInventoryIcon");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject* CInventoryIcon::Clone(void* pArg)
{
	CInventoryIcon* pGameInstance = new CInventoryIcon(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CInventoryIcon");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CInventoryIcon::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}