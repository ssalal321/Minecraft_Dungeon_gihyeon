#pragma once
#include "Model.h"
#include "VIBuffer.h"

BEGIN(Engine)
class CVIBuffer_Cube;

class ENGINE_DLL CMesh final : public CVIBuffer
{
private:
	// BVH_Node는 m_TriOrder[iTriStart ... iTriStart + iTriCount - 1]에 들어있는 삼각형만 담당한다!
	struct BVH_Node
	{
		_float3		boundingMin;	// 이 노드에 속한 삼각형들을 모두 감싸는 AABB
		_float3		boundingMax;

		_int		iLeft;			// 왼쪽 자식 노드 인덱스 (-1이면 leaf)
		_int		iRight;			// 오른쪽 자식 노드 인덱스 (-1이면 leaf)

		_int		iTriStart;		// m_TriOrder에서 이 노드가 담당하는 첫 삼각형 위치
		_int		iTriCount;		// 이 노드가 담당하는 삼각형 개수
	};


	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& Prototype);
	~CMesh() override = default;

public:
	_uint		Get_MaterialIndex() const { return m_iMaterialIndex; }

	void		Set_Pickable(_bool bPickable) { m_bPickable = bPickable; }

public:
	HRESULT Initialize_Prototype(CModel::TYPE eModelType, const vector<class CBone*>& Bones, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, const vector<class CBone*>& Bones);

	void	Build_BVH();

	// Local 상태의 min, max를 월드 상태로 변환, Collision_AABB 호출해 충돌 여부 반환
	_bool	Check_BoundingBox_AABB(const _float3& localMousePos, const _float3& localMouseRay);

	// 정적 모델 피킹용 (Picking_Triangle 호출)
	//_bool	Picking_In_World(const _float3& vMousePos, const _float3& vMouseRay, _float3& vPickedPos) const;

	// 동적 모델 피킹용 (Picking_Triangle 호출)
	_bool	Picking_In_Mesh(const _float3& localMousePos, const _float3& localMouseRay,
							_float3& vOutLocalPickedPos, _float& fOutDist) const;

	_bool	Picking_In_Mesh_with_BVH(const _float3& localMousePos, const _float3& localMouseRay,
							_float3& vOutLocalPickedPos, _float& fOutDist) const;

	_bool Intersect_Leaf_Triangles(_int triStart, _int triCount, const _vector& vOrigin, const _vector& vDir, _float& ioBestDist, _float3& outPicked) const;

	_bool	Picking_Vertex(const _float3& localMousePos, const _float3& localMouseRay,
							_float3& vOutPickedVertex, _float& fOutDist, _float fThreshold = 0.3f) const;

	_bool	Ray_Intersects_Sphere(const _float3& localMousePos, const _float3& localMouseRay, const _float3& sphereCenter,
								 _float sphereRadius, _float& outDistance) const;

private:
	_char				m_szName[MAX_PATH] = "";
	_uint				m_iMaterialIndex = {};
	_uint				m_iNumBones = {};
	_uint				m_iNumFaces = {};

	/* 피킹에 사용되는 변수들 */

	_float3				m_vBoundingMin = {};
	_float3				m_vBoundingMax = {};
	_float3*			m_pVertices = {};

	_uint*				m_pIndices = { nullptr };

	/* 이 메시에 영향을 주는 뼈들을 모아놓은 컨테이너 */
	/* 뼈 : 모델을 구성하는 전체 뼈 중, 몇번째 */
	vector<_uint>		m_Bones;

	_float4x4			m_BoneMatrices[g_iMaxNumBones] = {};
	vector<_float4x4>	m_OffsetMatrices;

	_bool				m_bPickable = { false };

#pragma region
	vector<BVH_Node>	m_BVHNodes;
	vector<_uint>		m_TriangleIDs;

#pragma endregion

private:
	HRESULT		Ready_VertexBuffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT		Ready_VertexBuffer_For_Anim(const aiMesh* pAIMesh, const vector<class CBone*>& Bones);
	void		Compute_BoundingBox();


#pragma region BVH
	_bool	Ray_Intersects_AABB(const _float3& rayOrigin, const _float3& rayDir,
		const _float3& bmin, const _float3& bmax,
		_float& tMinOut, _float& tMaxOut) const;

	_int		Build_BVH_Node(_int start, _int count, _int depth = 0);
	void		Compute_Node_AABB(_int start, _int count, _float3& outMin, _float3& outMax) const;
	static inline	void	Expand_AABB(_float3& mn, _float3& mx, const _float3& p);

	void		Compute_Centroid_Bounds(_int start, _int count, _float3& outMin, _float3& outMax) const;
	_int		Choose_Split_Axis(const _float3& cMin, const _float3& cMax) const;
	_int		Partition_Median(_int start, _int count, _int axis);
	_float3		Compute_Triangle_Centroid(_uint triId) const;
	_bool		Should_Stop_Splitting(_int count, const _float3& centroidExtent) const;
#pragma endregion

public:
	static	CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, 
						  const vector<class CBone*>& Bones, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	CComponent* Clone(void* pArg)	override;
	void	Free()					override;

};

END