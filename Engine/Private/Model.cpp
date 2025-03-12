#include "Model.h"
#include "Mesh.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent { pDevice, pContext }
{
}

CModel::CModel(const CModel& Prototype)
    : CComponent{ Prototype }
	, m_iNumMeshes { Prototype.m_iNumMeshes }
	, m_Meshes { Prototype.m_Meshes }
{
	for (auto& pMesh : m_Meshes)	
		Safe_AddRef(pMesh);	
}

HRESULT CModel::Initialize_Prototype(const _char* pModelFilePath)
{
	/* 어떤 설정? */
	/* 데이터를 읽을 때 설정값에 따라서 데이터를 조작하여 로드해준다. */
	_uint			iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;	

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	if (FAILED(Ready_Meshes()))
		return E_FAIL;

    return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CModel::Render()
{
	for (auto& pMesh : m_Meshes)
	{
		pMesh->Input_Assembler();
		pMesh->Render();
	}

	return S_OK;
}

HRESULT CModel::Ready_Meshes()
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_pAIScene->mMeshes[i]);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath)
{
	CModel* pGameInstance = new CModel(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype(pModelFilePath)))
	{
		MSG_BOX("Failed to Create : CModel");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CComponent* CModel::Clone(void* pArg)
{
	CModel* pGameInstance = new CModel(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CModel");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CModel::Free()
{
    __super::Free();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);

	m_Meshes.clear();

}
