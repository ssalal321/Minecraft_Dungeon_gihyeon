#include "Mesh.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CMesh::CMesh(const CMesh& Prototype)
	: CVIBuffer{ Prototype }
{
}

HRESULT CMesh::Initialize_Prototype(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	/* 네모를 구성하기위한 정점과 인덱스의 정보를 채우고 버퍼를 할당할 수 있도록 함수를 호출해준다. */
	m_iMaterialIndex	 = pAIMesh->mMaterialIndex;
	m_iNumVertexBuffers  = 1;
	m_iVertexStride		 = sizeof(VTXMESH);
	m_iNumVertices		 = pAIMesh->mNumVertices;
	m_iIndexStride		 = 4;
	m_iNumIndices		 = pAIMesh->mNumFaces * 3;
	m_eIndexFormat		 = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth			 = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage				 = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags			 = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags		 = 0;
	m_BufferDesc.MiscFlags			 = 0;

	VTXMESH*   pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition,
					XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal,
					XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix));

		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));

		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);
	m_InitialDesc.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth				= m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage					= D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags				= D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride	= m_iIndexStride;
	m_BufferDesc.CPUAccessFlags			= 0;
	m_BufferDesc.MiscFlags				= 0;

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = {};

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];
	}
	
	ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);
	m_InitialDesc.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	CMesh* pGameInstance = new CMesh(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype(pAIMesh, PreTransformMatrix)))
	{
		MSG_BOX("Failed to Create : CMesh");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CComponent* CMesh::Clone(void* pArg)
{
	CMesh* pGameInstance = new CMesh(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CMesh");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


void CMesh::Free()
{
	__super::Free();
}
