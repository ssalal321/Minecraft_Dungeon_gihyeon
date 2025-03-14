#include "Material.h"
#include "Shader.h"

CMaterial::CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CMaterial::Initialize(const aiMaterial* pAIMaterial, const _char* pModelFilePath)
{
	for (size_t i = 1; i < AI_TEXTURE_TYPE_MAX; i++)
	{
		_uint		iNumTextures = pAIMaterial->GetTextureCount(aiTextureType(i));

		m_SRVs[i].reserve(iNumTextures);

		for (size_t j = 0; j < iNumTextures; j++)
		{
			aiString	strTexturePath;

			if (FAILED(pAIMaterial->GetTexture(aiTextureType(i), j, &strTexturePath)))
				return E_FAIL;

			_char		szDrive[MAX_PATH] = {};
			_char		szDir[MAX_PATH] = {};
			_char		szFilename[MAX_PATH] = {};
			_char		szExt[MAX_PATH] = {};

			_char		szTextureFilePath[MAX_PATH] = {};

			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
			_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFilename, MAX_PATH, szExt, MAX_PATH);

			strcpy_s(szTextureFilePath, szDrive);
			strcat_s(szTextureFilePath, szDir);
			strcat_s(szTextureFilePath, szFilename);
			strcat_s(szTextureFilePath, szExt);

			_tchar		szFullPath[MAX_PATH] = {};

			MultiByteToWideChar(CP_ACP, 0, szTextureFilePath, strlen(szTextureFilePath),
				szFullPath, MAX_PATH);

			ID3D11ShaderResourceView* pSRV = { nullptr };

			if (false == strcmp(szExt, ".dds"))
			{
				if (FAILED(CreateDDSTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV)))
					return E_FAIL;
			}
			else if (false == strcmp(szExt, ".tga"))
				return E_FAIL;

			else
			{
				if (FAILED(CreateWICTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV)))
					return E_FAIL;
			}

			m_SRVs[i].push_back(pSRV);
		}
	}

	return S_OK;
}

HRESULT CMaterial::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, aiTextureType eMaterialType, _uint iTextureIndex)
{
	if (iTextureIndex >= m_SRVs[eMaterialType].size())
		return E_FAIL;

	return pShader->Bind_SRV(pConstantName, m_SRVs[eMaterialType][iTextureIndex]);
}

CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMaterial* pAIMaterial,
							 const _char* pModelFilePath)
{
	CMaterial* pGameInstance = new CMaterial(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize(pAIMaterial, pModelFilePath)))
	{
		MSG_BOX("Failed to Create : CMaterial");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CMaterial::Free()
{
	__super::Free();

	for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
	{
		for (auto& pSRV : m_SRVs[i])
			Safe_Release(pSRV);

		m_SRVs[i].clear();
	}

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
