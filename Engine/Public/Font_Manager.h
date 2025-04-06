#pragma once

#include "Base.h"

BEGIN(Engine)

class CFont_Manager final : public CBase
{
private:
	CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	~CFont_Manager() override = default;

public:
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);

private:
	map<const _wstring, class CCustomFont*>				m_Fonts;
	ID3D11Device*										m_pDevice = { nullptr };
	ID3D11DeviceContext*								m_pContext = { nullptr };

public:
	static CFont_Manager*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath);
	void	Free() override;
};

END