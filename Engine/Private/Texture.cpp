#include "Texture.h"

#include <random>
#include "Shader.h"

CTexture::CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent { pDevice, pContext }
{
}

CTexture::CTexture(const CTexture& Prototype)
    : CComponent { Prototype }
	, m_iNumSRVs { Prototype.m_iNumSRVs }
	, m_SRVs { Prototype.m_SRVs }
{
	for (auto& pSRV : m_SRVs)
		Safe_AddRef(pSRV);

}

HRESULT CTexture::Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures)
{
	/* ..\Bin\Resources\Textures\Explosion\Explosion%d.png, 90 */

	m_iNumSRVs = iNumTextures;

	m_SRVs.reserve(m_iNumSRVs);

	for (size_t i = 0; i < iNumTextures; i++)
	{
		_tchar		szTextureFilePath[MAX_PATH]{};

		ID3D11ShaderResourceView*	pSRV = { nullptr };

		wsprintf(szTextureFilePath, pTextureFilePath, i);

		_tchar		szEXT[MAX_PATH] = {};

		_wsplitpath_s(szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

		HRESULT		hr = {};

		if (0 == lstrcmp(szEXT, TEXT(".dds")))
			hr = CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);

		else if (0 == lstrcmp(szEXT, TEXT(".tga")))
			hr = E_FAIL;
		else
			hr = CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);

		if (FAILED(hr))
			return E_FAIL;			

		m_SRVs.push_back(pSRV);
	}

    return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, m_iNumSRVs - 1);

	m_iRandomIndex = dist(gen); // 랜덤 값 가져오기

    return S_OK;
}

HRESULT CTexture::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iTextureIndex)
{
	if (iTextureIndex >= m_iNumSRVs)
		return E_FAIL;

	if (m_iNumSRVs > 1)
	{
		return pShader->Bind_SRV(pConstantName, m_SRVs[m_iRandomIndex]);
	}
	else
		return pShader->Bind_SRV(pConstantName, m_SRVs[iTextureIndex]);	
}

CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures)
{
	CTexture* pGameInstance = new CTexture(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype(pTextureFilePath, iNumTextures)))
	{
		MSG_BOX("Failed to Create : CTexture");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CComponent* CTexture::Clone(void* pArg)
{
	CTexture* pGameInstance = new CTexture(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CTexture");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CTexture::Free()
{
    __super::Free();

	for (auto& pSRV : m_SRVs)
		Safe_Release(pSRV);

	m_SRVs.clear();

	
}
