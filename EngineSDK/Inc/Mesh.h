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

	_bool	Check_BoundingBox_Collsion(const _float3& vMouseRayPos, const _float3& vMouseRayDir,
										const _float4x4& WorldMatrix);
	_bool	Picking_AABB(const _float3& vRayOrigin, const _float3& vRayDir, const _float3& vWorldMin, const _float3& vWorldMax);
	_bool	Picking_In_World(const _float3& vMouseRay, const _float3& vMousePos, _float3& vPickedPos, const _float3& vPointA, const _float3&
		                      vPointB, const _float3& vPointC);
	_bool	Picking_In_Local(const _float3& vMouseRay, const _float3& vMousePos, _float3& vStoreLocalMouseRay, _float3& vStoreLocalMousePos, _float3& vPickedPos, const _float3& vPointA,
		                      const _float3& vPointB, const _float3& vPointC, const _float4x4& WorldMatrixInverse);

private:
	_char				m_szName[MAX_PATH] = "";
	_uint				m_iMaterialIndex = {};
	_uint				m_iNumBones = { 0 };
	_float3				m_vBoundingMin = {};
	_float3				m_vBoundingMax = {};
	aiVector3D*			m_pVertices = {};

	/* 이 메시에 영향을 주는 뼈들을 모아놓은 컨테이너 .*/
	/* 뼈 : 모델을 구성하는 전체 뼈 중, 몇번째 */
	vector<_uint>		m_Bones;

	_float4x4			m_BoneMatrices[g_iMaxNumBones] = {};
	vector<_float4x4>	m_OffsetMatrices;

private:
	HRESULT		Ready_VertexBuffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT		Ready_VertexBuffer_For_Anim(const aiMesh* pAIMesh, const vector<class CBone*>& Bones);
	void		Compute_BoundingBox();

	_bool		Picking_Triangle(_float3& vPickedPos, const _float3& vRayOrigin, const _float3& vRayDir,
			                      const _float3& vPointA,
			                      const _float3& vPointB, const _float3& vPointC);
	void		Transform_To_LocalSpace(const _float3& vMouseRay, const _float3& vMousePos, _float3& vLocalMouseRay, _float3& vLocalMousePos, const _float4x4&
			                            WorldMatrixInverse);

public:
	static	CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, 
						  const vector<class CBone*>& Bones, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	CComponent* Clone(void* pArg)	override;
	void	Free()					override;

};

END