#pragma once

#include "Model.h"
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& Prototype);
	~CMesh() override = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	HRESULT Initialize_Prototype(CModel::TYPE eModelType, const vector<class CBone*>& Bones, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, const vector<class CBone*>& Bones);

	// Local 상태의 min, max를 월드 상태로 변환, Collision_AABB 호출해 충돌 여부 반환
	_bool	Check_BoundingBox_Collision(const _float3& vMouseRayPos, const _float3& vMouseRayDir,
										const _float4x4& WorldMatrix);

	// ray와 BoundingBox간 min max Time 연산 후 충돌 여부 반환
	_bool	Collision_AABB(const _float3& vRayOrigin, const _float3& vRayDir, const _float3& vWorldMin, const _float3& vWorldMax);

	//// 정적 모델 피킹용 (Picking_Triangle 호출)
	//_bool	Picking_In_World(const _float3& vMousePos, const _float3& vMouseRay, _float3& vPickedPos) const;

	// 동적 모델 피킹용 (Picking_Triangle 호출)
	_bool	Picking_In_Mesh(const _float3& vMousePos, const _float3& vMouseRay, _float3& vPickedPos, const _float4x4& WorldMatrix) const;

private:
	_char				m_szName[MAX_PATH] = "";
	_uint				m_iMaterialIndex = {};
	_uint				m_iNumBones = {};
	_uint				m_iNumFaces = {};
	_float3				m_vBoundingMin = {};
	_float3				m_vBoundingMax = {};
	_float3*			m_pVertices = {};

	_uint*				m_pIndices = { nullptr };

	/* 이 메시에 영향을 주는 뼈들을 모아놓은 컨테이너 .*/
	/* 뼈 : 모델을 구성하는 전체 뼈 중, 몇번째 */
	vector<_uint>		m_Bones;

	_float4x4			m_BoneMatrices[g_iMaxNumBones] = {};
	vector<_float4x4>	m_OffsetMatrices;

private:
	HRESULT		Ready_VertexBuffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT		Ready_VertexBuffer_For_Anim(const aiMesh* pAIMesh, const vector<class CBone*>& Bones);
	void		Compute_BoundingBox();

	_bool		Picking_Triangle(_float3& vPickedPos, const _float3& vRayPos, const _float3& vRayDir) const;

public:
	static	CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, 
						  const vector<class CBone*>& Bones, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	CComponent* Clone(void* pArg)	override;
	void	Free()					override;

};

END