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

	// m_pIndices는 이 메시의 정점을 어떻게 읽을 건지 저장하는 인덱스 배열
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
			_uint	boneIndex	= static_cast<_uint>(i); // 현재 처리 중인 뼈 인덱스

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
				pVertices[iVertexIndex].vBlendIndex.x = static_cast<_uint>(i);
				pVertices[iVertexIndex].vBlendWeight.x = pAIBone->mWeights[j].mWeight;
			}
			else if (0.f == pVertices[iVertexIndex].vBlendWeight.y)
			{
				pVertices[iVertexIndex].vBlendIndex.y = static_cast<_uint>(i);
				pVertices[iVertexIndex].vBlendWeight.y = pAIBone->mWeights[j].mWeight;
			}
			
			else if (0.f == pVertices[iVertexIndex].vBlendWeight.z)
			{
				pVertices[iVertexIndex].vBlendIndex.z = static_cast<_uint>(i);
				pVertices[iVertexIndex].vBlendWeight.z = pAIBone->mWeights[j].mWeight;
			}
			else
			{
				pVertices[iVertexIndex].vBlendIndex.w = static_cast<_uint>(i);
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

void CMesh::Build_BVH()
{
	const _uint iTriCount = m_iNumIndices / 3;

	m_TriangleIDs.resize(iTriCount);
	
	for (_uint i = 0; i < iTriCount; ++i)
		m_TriangleIDs[i] = i;

	m_BVHNodes.clear();

	Build_BVH_Node(0, iTriCount);	// 루트 생성 (start = 0, count = triCount)
}

_bool CMesh::Ray_Intersects_AABB(const _float3& rayOrigin, const _float3& rayDir,
	const _float3& bmin, const _float3& bmax,
	_float& tMinOut, _float& tMaxOut) const
{
	_float tMin = 0.0f;
	_float tMax = FLT_MAX;

	const _float o[3] = { rayOrigin.x, rayOrigin.y, rayOrigin.z };
	const _float d[3] = { rayDir.x,    rayDir.y,    rayDir.z };
	const _float mn[3] = { bmin.x,      bmin.y,      bmin.z };
	const _float mx[3] = { bmax.x,      bmax.y,      bmax.z };

	for (int axis = 0; axis < 3; ++axis)
	{
		if (fabsf(d[axis]) < 1e-6f)
		{
			if (o[axis] < mn[axis] || o[axis] > mx[axis])
				return false;
		}
		else
		{
			_float t1 = (mn[axis] - o[axis]) / d[axis];
			_float t2 = (mx[axis] - o[axis]) / d[axis];
			if (t1 > t2) std::swap(t1, t2);

			tMin = (t1 > tMin) ? t1 : tMin;
			tMax = (t2 < tMax) ? t2 : tMax;

			if (tMin > tMax)
				return false;
		}
	}

	tMinOut = tMin;
	tMaxOut = tMax;
	return true;
}


_int CMesh::Build_BVH_Node(_int iStart, _int iTriCount, _int depth)
{
	// 디버그용
	std::string indent(depth * 2, ' ');

	// 입장 로그
	printf("%s[Enter] start=%d count=%d\n", indent.c_str(), iStart, iTriCount);

	BVH_Node node{};
	node.iTriStart = iStart;
	node.iTriCount = iTriCount;
	node.iLeft = -1;
	node.iRight = -1;

	// 1) bounds
	Compute_Node_AABB(iStart, iTriCount, node.boundingMin, node.boundingMax);

	// 2) centroid bounds
	_float3 ctMin, ctMax;
	Compute_Centroid_Bounds(iStart, iTriCount, ctMin, ctMax);

	_float3 centroidExtent{ ctMax.x - ctMin.x, ctMax.y - ctMin.y, ctMax.z - ctMin.z };

	// 3) 분할 끝까지 왔는가? 리프노드인가?
	// Build_BVH_Node()는 재귀함수이다
	if (Should_Stop_Splitting(iTriCount, centroidExtent))
	{
		const _int idx = static_cast<_int>(m_BVHNodes.size());
		m_BVHNodes.push_back(node);

		// 디버그용
		printf("%s  [Leaf]  idx=%d  AABBMin(%.2f %.2f %.2f) Max(%.2f %.2f %.2f)\n",
			indent.c_str(), idx,
			node.boundingMin.x, node.boundingMin.y, node.boundingMin.z,
			node.boundingMax.x, node.boundingMax.y, node.boundingMax.z);

		return idx;
	}

	// 4) split
	const _int axis = Choose_Split_Axis(ctMin, ctMax);
	const _int mid = Partition_Median(iStart, iTriCount, axis);

	// 디버그용
	const int leftCount = mid - iStart;
	const int rightCount = (iStart + iTriCount) - mid;

	printf("%s  [Split] axis=%c mid=%d  left=%d right=%d  extent(%.3g %.3g %.3g)\n",
		indent.c_str(),
		(axis == 0 ? 'X' : axis == 1 ? 'Y' : 'Z'),
		mid, leftCount, rightCount,
		centroidExtent.x, centroidExtent.y, centroidExtent.z);

	if (leftCount == 0 || rightCount == 0)
	{
		printf("%s  [WARN] Degenerate split! axis=%c start=%d count=%d mid=%d\n",
			indent.c_str(), (axis == 0 ? 'X' : axis == 1 ? 'Y' : 'Z'), iStart, iTriCount, mid);
	}

	// 5) push self, build children
	const _int myIndex = (_int)m_BVHNodes.size();
	m_BVHNodes.push_back(node);

	// 디버그용
	printf("%s  [Push ] idx=%d\n", indent.c_str(), myIndex);

	m_BVHNodes[myIndex].iLeft = Build_BVH_Node(iStart, mid - iStart);
	m_BVHNodes[myIndex].iRight = Build_BVH_Node(mid, (iStart + iTriCount) - mid);

	// 디버그용
	printf("%s[Exit ] idx=%d left=%d right=%d\n",
		indent.c_str(), myIndex,
		m_BVHNodes[myIndex].iLeft,
		m_BVHNodes[myIndex].iRight);

	return myIndex;
}

void CMesh::Compute_Node_AABB(_int iStart, _int iTriCount, _float3& boundingMin, _float3& boundingMax) const
{
	// iStart : m_TriOrders에서 몇 번째부터
	// iTriCount : 삼각형 몇 개를 골라서
	// 그 삼각형들을 감싸는 AABB를 boundingMin/Max에 채운다.

	// m_TriOrders : 삼각형 ID 목록

	boundingMin = { FLT_MAX, FLT_MAX, FLT_MAX };
	boundingMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

	for (_int i = 0; i < iTriCount; ++i)
	{
		const _uint triangleID = m_TriangleIDs[iStart + i];
		const _uint base = triangleID * 3;

		// i0 ~ i2 : 하나의 삼각형을 이룰 정점 번호
		// 삼각형 구성은 인덱스가 정의하므로 인덱스를 받고 그로 정점에 접근해야 함
		const _uint i0 = m_pIndices[base + 0];
		const _uint i1 = m_pIndices[base + 1];
		const _uint i2 = m_pIndices[base + 2];

		// m_pVertices[i0]는 : 실제 정점 번호임
		Expand_AABB(boundingMin, boundingMax, m_pVertices[i0]);
		Expand_AABB(boundingMin, boundingMax, m_pVertices[i1]);
		Expand_AABB(boundingMin, boundingMax, m_pVertices[i2]);
	}
}

inline void CMesh::Expand_AABB(_float3& mn, _float3& mx, const _float3& pVertice)
{
	mn.x = min(mn.x, pVertice.x);
	mn.y = min(mn.y, pVertice.y);
	mn.z = min(mn.z, pVertice.z);
	
	mx.x = max(mx.x, pVertice.x);
	mx.y = max(mx.y, pVertice.y);
	mx.z = max(mx.z, pVertice.z);
}

void CMesh::Compute_Centroid_Bounds(_int start, _int count, _float3& ctMin, _float3& ctMax) const
{
	ctMin = { FLT_MAX, FLT_MAX, FLT_MAX };
	ctMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

	for (_int i = 0; i < count; ++i)
	{
		const _uint triangleID = m_TriangleIDs[start + i];
		const _float3 centroid = Compute_Triangle_Centroid(triangleID);

		Expand_AABB(ctMin, ctMax, centroid);
	}
}

_int CMesh::Choose_Split_Axis(const _float3& cMin, const _float3& cMax) const
{
	const _float3 extent{ cMax.x - cMin.x, cMax.y - cMin.y, cMax.z - cMin.z };

	_int axis = 0;
	if (extent.y > extent.x) axis = 1;
	if (extent.z > (axis == 0 ? extent.x : extent.y)) axis = 2;
	return axis;
}

_int CMesh::Partition_Median(_int iStart, _int iTriCount, _int iAxis)
{
	// m_TriOrders[start ~ start + count) 구간을
	// centroid의 axis 값 기준으로 반으로 나눠놓기
	const _int mid = iStart + iTriCount / 2;

	auto key = [&](const _uint triangleID) -> _float
		{
			const _float3 centroid = Compute_Triangle_Centroid(triangleID);
			return (iAxis == 0) ? centroid.x : (iAxis == 1 ? centroid.y : centroid.z);
		};

	// nth는 : 정렬했을 때 그 자리에 올 원소
	// first ~ nth 구간은 nth보다 작은 것들
	// nth ~ last 구간은 nth보다 큰 것들
	// 단, 각 구간 내부가 정렬되어 있진 않음
	std::nth_element(
		m_TriangleIDs.begin() + iStart,
		m_TriangleIDs.begin() + mid,
		m_TriangleIDs.begin() + (iStart + iTriCount),
		[&](const _uint a, const _uint b) { return key(a) < key(b); }
	);

	return mid;
}

_float3 CMesh::Compute_Triangle_Centroid(_uint iTriangleID) const
{
	const _uint base = iTriangleID * 3;
	const _uint i0 = m_pIndices[base + 0];
	const _uint i1 = m_pIndices[base + 1];
	const _uint i2 = m_pIndices[base + 2];

	const _float3& vA = m_pVertices[i0];
	const _float3& vB = m_pVertices[i1];
	const _float3& vC = m_pVertices[i2];

	return { (vA.x + vB.x + vC.x) * 0.333333f,
			 (vA.y + vB.y + vC.y) * 0.333333f,
			 (vA.z + vB.z + vC.z) * 0.333333f };
}

bool CMesh::Should_Stop_Splitting(_int count, const _float3& centroidExtent) const
{
	// 함수가 true를 반환하면:
	// 이 노드는 더 이상 분할하지 않는다
	// = leaf 노드가 된다

	constexpr int LEAF_TRI_MAX = 8;	// consteval 써도 될 듯?
	if (count <= LEAF_TRI_MAX)
		return true;

	// 더 쪼개도 의미 없는 경우(centroid가 거의 동일할 때)
	// 이때 나누면 오히려 반으로 잘 안나뉘어서 재귀만 깊어짐
	const float eps = 1e-6f;
	if (centroidExtent.x < eps && centroidExtent.y < eps && centroidExtent.z < eps)
		return true;

	return false;
}

_bool CMesh::Check_BoundingBox_AABB(const _float3& localMousePos, const _float3& localMouseRay)
{
	if (false == m_bPickable)
		return false;

	_float tMin = 0.0f, tMax = FLT_MAX;

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

_bool CMesh::Picking_In_Mesh_with_BVH(const _float3& localMousePos, const _float3& localMouseRay,
	_float3& vOutLocalPickedPos, _float& fOutDist) const
{
	if (!m_bPickable)
		return false;

	if (m_BVHNodes.empty())
		return Picking_In_Mesh(localMousePos, localMouseRay, vOutLocalPickedPos, fOutDist); // fallback

	_vector vOrigin = XMLoadFloat3(&localMousePos);
	_vector vDir = XMVector3Normalize(XMLoadFloat3(&localMouseRay));

	_float bestDist = FLT_MAX;
	_bool  hit = false;

	// 루트 AABB부터 빠르게 컷 (모델 쪽에서 이미 BB 체크하지만 안전하게 한 번 더 가능)
	_float tminRoot = 0.f, tmaxRoot = FLT_MAX;
	if (!Ray_Intersects_AABB(localMousePos, localMouseRay,
		m_BVHNodes[0].boundingMin, m_BVHNodes[0].boundingMax,
		tminRoot, tmaxRoot))
		return false;

	// 스택: (nodeIndex, tMin) 정도만 들고 가도 되지만 단순화
	std::vector<_int> stack;
	stack.reserve(64);
	stack.push_back(0);

	while (!stack.empty())
	{
		const _int nodeIdx = stack.back();
		stack.pop_back();

		const BVH_Node& node = m_BVHNodes[nodeIdx];

		_float tmin = 0.f, tmax = FLT_MAX;
		if (!Ray_Intersects_AABB(localMousePos, localMouseRay,
			node.boundingMin, node.boundingMax,
			tmin, tmax))
			continue;

		// 이미 더 가까운 hit를 찾았으면, 그보다 먼 노드는 볼 필요 없음
		// (tmin이 bestDist보다 크면 이 노드 안에서 더 가까운 hit가 나올 수 없음)
		if (hit && tmin > bestDist)
			continue;

		const _bool isLeaf = (node.iLeft < 0 && node.iRight < 0);

		if (isLeaf)
		{
			_float3 picked = {};
			if (Intersect_Leaf_Triangles(node.iTriStart, node.iTriCount, vOrigin, vDir, bestDist, picked))
			{
				hit = true;
				vOutLocalPickedPos = picked;
			}
		}
		else
		{
			// 자식 둘 다 넣되, "가까운 쪽을 먼저" 처리하면 더 빨리 bestDist가 줄어서 pruning이 좋아짐
			// 그래서 각 자식의 tmin을 구해 정렬 비슷하게 처리
			_int left = node.iLeft;
			_int right = node.iRight;

			if (left >= 0 && right >= 0)
			{
				_float lmin = 0.f, lmax = FLT_MAX, rmin = 0.f, rmax = FLT_MAX;
				_bool lhit = Ray_Intersects_AABB(localMousePos, localMouseRay,
					m_BVHNodes[left].boundingMin, m_BVHNodes[left].boundingMax,
					lmin, lmax);
				_bool rhit = Ray_Intersects_AABB(localMousePos, localMouseRay,
					m_BVHNodes[right].boundingMin, m_BVHNodes[right].boundingMax,
					rmin, rmax);

				if (lhit && rhit)
				{
					// 스택은 LIFO라 "먼 쪽 먼저 push"하면 가까운 쪽이 먼저 pop됨
					if (lmin < rmin)
					{
						stack.push_back(right);
						stack.push_back(left);
					}
					else
					{
						stack.push_back(left);
						stack.push_back(right);
					}
				}
				else
				{
					if (lhit) stack.push_back(left);
					if (rhit) stack.push_back(right);
				}
			}
			else
			{
				if (left >= 0)  stack.push_back(left);
				if (right >= 0) stack.push_back(right);
			}
		}
	}

	if (hit)
	{
		fOutDist = bestDist;
		return true;
	}
	return false;
}

_bool CMesh::Intersect_Leaf_Triangles(_int triStart, _int triCount,
	const _vector& vOrigin, const _vector& vDir,
	_float& ioBestDist, _float3& outPicked) const
{
	_bool hit = false;

	for (_int k = 0; k < triCount; ++k)
	{
		const _uint triId = m_TriangleIDs[triStart + k];
		const _uint base = triId * 3;

		const _uint i0 = m_pIndices[base + 0];
		const _uint i1 = m_pIndices[base + 1];
		const _uint i2 = m_pIndices[base + 2];

		const _vector v0 = XMLoadFloat3(&m_pVertices[i0]);
		const _vector v1 = XMLoadFloat3(&m_pVertices[i1]);
		const _vector v2 = XMLoadFloat3(&m_pVertices[i2]);

		_float dist = 0.f;
		if (TriangleTests::Intersects(vOrigin, vDir, v0, v1, v2, dist))
		{
			if (dist < ioBestDist)
			{
				ioBestDist = dist;
				XMStoreFloat3(&outPicked, vOrigin + vDir * dist);
				hit = true;
			}
		}
	}
	return hit;
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
