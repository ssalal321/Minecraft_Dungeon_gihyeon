#include "CustomFont.h"

CCustomFont::CCustomFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCustomFont::Initialize(const _tchar* pFontFilePath)
{
	m_pFont = new SpriteFont(m_pDevice, pFontFilePath);

	m_pBatch = new SpriteBatch(m_pContext);

	return S_OK;
}

/* 
_In_ SpriteBatch* spriteBatch, 
_In_z_ wchar_t const* text, 
XMFLOAT2 const& position, 
FXMVECTOR color = Colors::White, 
float rotation = 0, 
XMFLOAT2 const& origin = Float2Zero, 
float scale = 1, 
SpriteEffects effects = SpriteEffects_None, 
float layerDepth = 0
*/
HRESULT CCustomFont::Render(const _tchar* pText, const _float2& vPosition, _fvector vColor, _float fRotation, const _float2& vOrigin, _float fScale)
{
	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pBatch->Begin();

	m_pFont->DrawString(m_pBatch, pText, vPosition, vColor, fRotation, vOrigin, fScale);

	m_pBatch->End();

	return S_OK;
}

CCustomFont* CCustomFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath)
{
	CCustomFont* pInstance = new CCustomFont(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pFontFilePath)))
	{
		MSG_BOX("Failed to Created : CCustomFont");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCustomFont::Free()
{
	__super::Free();

	Safe_Delete(m_pBatch);
	Safe_Delete(m_pFont);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
