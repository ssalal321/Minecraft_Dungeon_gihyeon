#include "Font_Manager.h"

CFont_Manager::CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
}

HRESULT CFont_Manager::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
	return E_NOTIMPL;
}

CFont_Manager* CFont_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath)
{
	return nullptr;
}

void CFont_Manager::Free()
{
}
