#include "Mesh.h"

#include <set>

#include "Bone.h"
#include "Shader.h"
#include "VIBuffer_Cube.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer (pDevice, pContext)
{
}

CMesh::CMesh(const CMesh& Prototype)
	: CVIBuffer(Prototype)
{
}

HRESULT CMesh::Initialize_Prototype(CModel::TYPE eModelType, const vector<class CBone*>& Bones, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	/* 네모를 구성하기위한 정점과 인덱스의 정보를 채우고 버퍼를 할당할 수 있도록 함수를 호출해준다. */
	strcpy_s(m_szName, pAIMesh->mName.data);
	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	m_iNumVertexBuffers = 1;
	m_iNumFaces = pAIMesh->mNumFaces;
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

	m_pIndices = new _uint[m_iNumIndices];
	ZeroMemory(m_pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = {};

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		m_pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
		m_pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
		m_pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];
	}
	
	ZeroMemory(&m_InitialDesc, sizeof m_InitialDesc);
	m_InitialDesc.pSysMem = m_pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

#pragma endregion

	Compute_BoundingBox();

	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	MESH_DESC* pDesc = static_cast<MESH_DESC*>(pArg);
	m_bPickable = pDesc->bPickable;

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

	m_pVertices = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(_float3) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));
		m_pVertices[i] = pVertices[i].vPosition;

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

	m_pVertices = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(_float3) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		m_pVertices[i] = pVertices[i].vPosition;

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


_bool CMesh::Check_BoundingBox_AABB(const _float3& localMousePos, const _float3& localMouseRay)
{
	if (false == m_bPickable)
		return false;

	_float tMin = 0.0f, tMax = FLT_MAX;

	/*if ( -56.f > m_vBoundingMin.x || m_vBoundingMax.x > 59.f ||
		  11.f > m_vBoundingMin.y || m_vBoundingMax.y > 89.f ||
		 -74.f > m_vBoundingMin.z || m_vBoundingMax.z > 75.f )
		return false;*/

	_float rayOrigin[3] = { localMousePos.x, localMousePos.y, localMousePos.z };
	_float rayDir[3] = { localMouseRay.x, localMouseRay.y, localMouseRay.z };
	_float fLocalMin[3] = { m_vBoundingMin.x, m_vBoundingMin.y, m_vBoundingMin.z };
	_float fLocalMax[3] = { m_vBoundingMax.x, m_vBoundingMax.y, m_vBoundingMax.z };

	// rayDir는 정규화된 상태
	for (int i = 0; i < 3; i++)  // X, Y, Z 축에 대해 검사
	{
		if (abs(rayDir[i]) < 1e-6f)
		{
			if (rayOrigin[i] < fLocalMin[i] || rayOrigin[i] > fLocalMax[i])
				return false;
		}
		else
		{
			_float t1 = (fLocalMin[i] - rayOrigin[i]) / rayDir[i];
			_float t2 = (fLocalMax[i] - rayOrigin[i]) / rayDir[i];

			if (t1 > t2)
				swap(t1, t2);

			tMin = max(tMin, t1);
			tMax = min(tMax, t2);

			if (tMin > tMax)
				return false;
		}
	}

	return true;
}


_bool CMesh::Picking_In_Mesh(const _float3& localMousePos, const _float3& localMouseRay,
							_float3& vOutLocalPickedPos, _float& fOutDist) const
{
	_vector  vOrigin = XMLoadFloat3(&localMousePos);
	_vector  vDir = XMLoadFloat3(&localMouseRay);

	_bool	bHit = false;
	_float	fMinDist = FLT_MAX;

	_float fDist = {};
	for (_uint i = 0; i < m_iNumIndices; i += 3)
	{
		_float3 vA = m_pVertices[m_pIndices[i + 0]];
		_float3 vB = m_pVertices[m_pIndices[i + 1]];
		_float3 vC = m_pVertices[m_pIndices[i + 2]];

		/*_float minX = min(vA.x, min(vB.x, vC.x));
		_float maxX = max(vA.x, max(vB.x, vC.x));*/
		if (vA.x < -40.f || vA.x > 40.f)
			continue;

		/*_float minZ = min(vA.z, min(vB.z, vC.z));
		_float maxZ = max(vA.z, max(vB.z, vC.z));*/
		if (vA.z < -50.f || vA.z > 55.f)
			continue;

		/*_float centerY = (vA.y + vB.y + vC.y) * 0.3333f;*/
		if (vA.y < 45.f || vA.y > 65.f)
			continue;	


		_vector v0 = XMLoadFloat3(&vA);
		_vector v1 = XMLoadFloat3(&vB);
		_vector v2 = XMLoadFloat3(&vC);

		//_float fDist;
		if (TriangleTests::Intersects(vOrigin, vDir, v0, v1, v2, fDist))
		{
			if (fDist < fMinDist)
			{
				fMinDist = fDist;
				XMStoreFloat3(&vOutLocalPickedPos, vOrigin + vDir * fDist);
				fOutDist = fDist;

				bHit = true;
			}
		}
	}

	return bHit;
}

_bool CMesh::Picking_Vertex(const _float3& localMousePos, const _float3& localMouseRay,
							_float3& vOutPickedVertex, _float& fOutDist, _float fThresholdRadius) const
{
	_vector  vDir		= XMVector3Normalize(XMLoadFloat3(&localMouseRay));
	_float3  vRay = {};
	XMStoreFloat3(&vRay, vDir);

	_bool	bHit		= false;
	_float	fMinDist	= FLT_MAX;

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		_float		fDist = {};

		if (Ray_Intersects_Sphere(localMousePos, vRay, m_pVertices[i], fThresholdRadius, fDist))
		{
			if (fDist < fMinDist)
			{
				fMinDist = fDist;
				XMStoreFloat3(&vOutPickedVertex, XMLoadFloat3(&m_pVertices[i]));
				fOutDist = fDist;
				bHit = true;
			}
		}
	}

	return bHit;
}

_bool CMesh::Ray_Intersects_Sphere(const _float3& localMousePos, const _float3& localMouseRay, const _float3& sphereCenter,
									_float sphereRadius, _float& outDistance) const 
{
	// 레이 시작점에서 교차 지점까지의 거리
	_vector  localMouseOrigin = XMLoadFloat3(&localMousePos);
	_vector  localRayDir = XMVector3Normalize(XMLoadFloat3(&localMouseRay));
	_vector  center = XMLoadFloat3(&sphereCenter);

	_vector  toCenter = center - localMouseOrigin;

	// 레이 방향으로 중심까지 정사영한 길이 (t)
	_float  projectionLength = XMVectorGetX(XMVector3Dot(toCenter, localRayDir));

	// sphere가 ray 방향과 반대일 경우
	if (projectionLength < 0.f)
		return false;

	_vector  closestPoint	= localMouseOrigin + localRayDir * projectionLength;
	_vector  diff			= center - closestPoint; // 가장 가까운 점과 중심 사이 거리

	_float	sqDistToCenter = XMVectorGetX(XMVector3LengthSq(diff)); // diff 길이 구하는 공식, LengthSq = (x² + y² + z²) 형태(루트 X)
	_float	radiusSquared  = sphereRadius * sphereRadius; // 반지름 제곱

	if (sqDistToCenter > radiusSquared)  // 루트보다 제곱이 연산이 덜 걸리므로 이렇게..
		return false;

	// 교차 확인, 교차 거리 반환
	outDistance = projectionLength;  // ray 상에서 얼마나 앞에 있나를 판단
	return true;
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

	Safe_Delete_Array(m_pIndices);
	Safe_Delete_Array(m_pVertices);
}
