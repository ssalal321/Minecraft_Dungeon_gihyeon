#include "Mesh.h"

#include <set>

#include "Bone.h"
#include "Shader.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CMesh::CMesh(const CMesh& Prototype)
	: CVIBuffer{ Prototype }
{
}

HRESULT CMesh::Initialize_Prototype(CModel::TYPE eModelType, const vector<class CBone*>& Bones, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	/* 네모를 구성하기위한 정점과 인덱스의 정보를 채우고 버퍼를 할당할 수 있도록 함수를 호출해준다. */
	strcpy_s(m_szName, pAIMesh->mName.data);
	m_pVertices = pAIMesh->mVertices;
	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	m_iNumVertexBuffers = 1;	
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iIndexStride = 4;
	m_iNumIndices = pAIMesh->mNumFaces * 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	HRESULT hr = CModel::TYPE_NONANIM == eModelType ?
		Ready_VertexBuffer_For_NonAnim(pAIMesh, PreTransformMatrix) :
		Ready_VertexBuffer_For_Anim(pAIMesh, Bones);

	if (FAILED(hr))
		return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iIndexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

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

	Compute_BoundingBox();

	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, const vector<class CBone*>& Bones)
{
	ZeroMemory(m_BoneMatrices, sizeof(_float4x4) * g_iMaxNumBones);

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		XMStoreFloat4x4(&m_BoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * Bones[m_Bones[i]]->Get_CombinedTransformationMatrix());
	}

	return pShader->Bind_Matrices(pConstantName, m_BoneMatrices, m_iNumBones);
}

HRESULT CMesh::Ready_VertexBuffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
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

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_For_Anim(const aiMesh* pAIMesh, const vector<class CBone*>& Bones)
{
	m_iVertexStride = sizeof(VTXANIMMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));

		/*pVertices[i].vBlendIndex;
		pVertices[i].vBlendWeight;*/
	}

	/* 이 메시에 영향을 주는 뼈의 갯수 */
	m_iNumBones = pAIMesh->mNumBones;

	m_OffsetMatrices.reserve(m_iNumBones);

	/* 뼈를 기준으로 정점들에게 정보를 채워준다. */
	for (size_t i = 0; i < m_iNumBones; i++)
	{
		/*0번째 뼈는 어떤 정점에게 영향을 줍니다. 
			얼마나 영향을 줍니다. */
		aiBone*		pAIBone = pAIMesh->mBones[i];

		_uint		iBoneIndex = {};

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool 
		{
				if (0 == strcmp(pBone->Get_Name(), pAIBone->mName.data))
					return true;

				++iBoneIndex;

				return false;			
		});

		/* 이 메시에 영향을 주는 뼈가 모델 전체 뼈 기준 몇번째에 들어가 있었는지(iIndex)를 모아놨다. */
		m_Bones.push_back(iBoneIndex);

		_float4x4		OffsetMatrix{};

		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatrices.push_back(OffsetMatrix);

		/* 이 메시에 영향을 주는 i번째 뼈는 몇개의 정점에게 영향을 주는가? */
		_uint		iNumWeights = pAIBone->mNumWeights;

		for (size_t j = 0; j < iNumWeights; j++) 
		{
			/* i번째 뼈가 영향을 주는 j번째 정점의 인덱스 :  pAIBone->mWeights[j].mVertexId */
			_uint	iVertexIndex = pAIBone->mWeights[j].mVertexId;

			/* 중복된 뼈인지 체크하기 위한 배열 */
			_uint	boneIndex	= i; // 현재 처리 중인 뼈 인덱스

			/* 현재 정점의 기존 블렌드 인덱스들과 비교 */
			// std::set -> 중복을 자동 제거하는 컨테이너
			set<_uint> existingBones = 
			{
				pVertices[iVertexIndex].vBlendIndex.x,
				pVertices[iVertexIndex].vBlendIndex.y,
				pVertices[iVertexIndex].vBlendIndex.z,
				pVertices[iVertexIndex].vBlendIndex.w
			};

			/* 중복된 뼈라면 추가하지 않음 */
			if (existingBones.find(boneIndex) != existingBones.end())
				continue;

			/* 네개 중에 아직 값이 채워지지 않은 공간을 찾는다. */
			if (0.f == pVertices[iVertexIndex].vBlendWeight.x)
			{
				pVertices[iVertexIndex].vBlendIndex.x = i;
				pVertices[iVertexIndex].vBlendWeight.x = pAIBone->mWeights[j].mWeight;
			}
			else if (0.f == pVertices[iVertexIndex].vBlendWeight.y)
			{
				pVertices[iVertexIndex].vBlendIndex.y = i;
				pVertices[iVertexIndex].vBlendWeight.y = pAIBone->mWeights[j].mWeight;
			}
			
			else if (0.f == pVertices[iVertexIndex].vBlendWeight.z)
			{
				pVertices[iVertexIndex].vBlendIndex.z = i;
				pVertices[iVertexIndex].vBlendWeight.z = pAIBone->mWeights[j].mWeight;
			}
			else
			{
				pVertices[iVertexIndex].vBlendIndex.w = i;
				pVertices[iVertexIndex].vBlendWeight.w = pAIBone->mWeights[j].mWeight;
			}
		}
	}

	ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);
	m_InitialDesc.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		_uint		iBoneIndex = {};

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
			{
				if (0 == strcmp(pBone->Get_Name(), m_szName))
					return true;

				++iBoneIndex;

				return false;
			});

		m_Bones.push_back(iBoneIndex);

		_float4x4	OffsetMatrix{};

		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_OffsetMatrices.push_back(OffsetMatrix);
	}

	Safe_Delete_Array(pVertices);

	return S_OK;
}

void CMesh::Compute_BoundingBox()
{
	// 최소, 최대값을 큰 값과 작은 값으로 초기화
	_float3 vMin = { FLT_MAX, FLT_MAX, FLT_MAX };
	_float3 vMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

	// 모든 정점을 순회하면서 AABB 계산
	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		const _float3 vPos = { m_pVertices[i].x, m_pVertices[i].y, m_pVertices[i].z, };

		vMin.x = min(vMin.x, vPos.x);
		vMin.y = min(vMin.y, vPos.y);
		vMin.z = min(vMin.z, vPos.z);

		vMax.x = max(vMax.x, vPos.x);
		vMax.y = max(vMax.y, vPos.y);
		vMax.z = max(vMax.z, vPos.z);
	}

	m_vBoundingMin = vMin;
	m_vBoundingMax = vMax;
}


_bool CMesh::Picking_Triangle(_float3& vPickedPos, const _float3& vRayOrigin, const _float3& vRayDir,
							  const _float3& vPointA, const _float3& vPointB, const _float3& vPointC)
{
	_float fDist;

	_vector  vOrigin = XMLoadFloat3(&vRayOrigin);
	_vector  vDir = XMLoadFloat3(&vRayDir);
	_vector  vA = XMLoadFloat3(&vPointA);
	_vector  vB = XMLoadFloat3(&vPointB);
	_vector  vC = XMLoadFloat3(&vPointC);

	_bool isPicked = TriangleTests::Intersects(vOrigin, vDir, vA, vB, vC, fDist);

	if (isPicked)
	{
		XMStoreFloat3(&vPickedPos, vOrigin + vDir * fDist);
	}

	return isPicked;
}

_bool CMesh::Check_BoundingBox_Collsion(const _float3& vMouseRayPos, const _float3& vMouseRayDir, const _float4x4& WorldMatrix)
{
	_float3 vMin, vMax;
	XMStoreFloat3(&vMin, XMVector3TransformCoord(XMLoadFloat3(&m_vBoundingMin), XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat3(&vMax, XMVector3TransformCoord(XMLoadFloat3(&m_vBoundingMax), XMLoadFloat4x4(&WorldMatrix)));

	return Picking_AABB(vMouseRayPos, vMouseRayDir, vMin, vMax);
}

_bool CMesh::Picking_AABB(const _float3& vRayOrigin, const _float3& vRayDir, const _float3& vWorldMin, const _float3& vWorldMax)
{
	_float tMin = 0.0f, tMax = FLT_MAX;

	std::vector<_float>		rayOrigin, rayDir, AABBmin, AABBmax;
	rayOrigin.push_back(vRayOrigin.x);
	rayOrigin.push_back(vRayOrigin.y);
	rayOrigin.push_back(vRayOrigin.z);

	rayDir.push_back(vRayDir.x);
	rayDir.push_back(vRayDir.y);
	rayDir.push_back(vRayDir.z);

	AABBmin.push_back(vWorldMin.x);
	AABBmin.push_back(vWorldMin.y);
	AABBmin.push_back(vWorldMin.z);

	AABBmax.push_back(vWorldMax.x);
	AABBmax.push_back(vWorldMax.y);
	AABBmax.push_back(vWorldMax.z);

	for (int i = 0; i < 3; i++)  // X, Y, Z 축에 대해 검사
	{
		if (abs(rayDir[i]) < 1e-6f)
		{
			if (rayOrigin[i] < AABBmin[i] || rayOrigin[i] > AABBmax[i])
				return false;
		}
		else
		{
			_float t1 = (AABBmin[i] - rayOrigin[i]) / rayDir[i];
			_float t2 = (AABBmax[i] - rayOrigin[i]) / rayDir[i];

			if (t1 > t2) swap(t1, t2);

			tMin = max(tMin, t1);
			tMax = min(tMax, t2);

			if (tMin > tMax)
				return false;
		}
	}

	return true;	// BoundingBox AABB 충돌 시 true 반환
}

_bool CMesh::Picking_In_World(const _float3& vMouseRay, const _float3& vMousePos, _float3& vPickedPos, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC)
{
	return Picking_Triangle(vPickedPos, vMousePos, vMouseRay, vPointA, vPointB, vPointC);
}

_bool CMesh::Picking_In_Local(const _float3& vMouseRay, const _float3& vMousePos, _float3& vStoreLocalMouseRay, _float3& vStoreLocalMousePos, _float3& vPickedPos, const _float3& vPointA, const _float3& vPointB,
	const _float3& vPointC, const _float4x4& WorldMatrixInverse)
{
	// 월드 -> 로컬 변환
	Transform_To_LocalSpace(vMouseRay, vMousePos, vStoreLocalMouseRay, vStoreLocalMousePos, WorldMatrixInverse);

	// 로컬 좌표에서 피킹 실행
	return Picking_Triangle(vPickedPos, vStoreLocalMousePos, vStoreLocalMouseRay, vPointA, vPointB, vPointC);
}

void CMesh::Transform_To_LocalSpace(const _float3& vMouseRay, const _float3& vMousePos, 
									_float3& vLocalMouseRay, _float3& vLocalMousePos, 
									const _float4x4& WorldMatrixInverse)
{
	_matrix  InvWorldMatrix = XMLoadFloat4x4(&WorldMatrixInverse);
	XMStoreFloat3(&vLocalMousePos, XMVector3TransformCoord(XMLoadFloat3(&vMousePos), InvWorldMatrix));

	_vector LocalMouseRay = XMVector3TransformNormal(XMLoadFloat3(&vMouseRay), InvWorldMatrix);
	XMStoreFloat3(&vLocalMouseRay, XMVector3Normalize(LocalMouseRay));		// 방향 벡터 정규화
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const vector<CBone*>& Bones, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	CMesh* pGameInstance = new CMesh(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype(eModelType, Bones, pAIMesh, PreTransformMatrix)))
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
