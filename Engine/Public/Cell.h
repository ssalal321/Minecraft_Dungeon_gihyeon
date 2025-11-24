#pragma once

#include "Base.h"

BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };

private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	~CCell() override = default;
	
public:
	_vector		Get_Point(POINT ePoint) { return XMLoadFloat3(&m_vPoints[ePoint]); }

	const _float3* Get_Points() { return m_vPoints; }

	const _float& Get_Plane_NormalY() { return m_vPlane.y; }

	const _vector&  Get_EdgeNormal(_int iIndex) { return XMVector3Normalize(XMLoadFloat3(&m_vNormals[iIndex])); }

	std::string		Get_CellKey() { return m_CellKey; }

	void	SetUp_Neighbor(LINE eLine, CCell* pNeighbor)
	{
		m_iNeighborCellIndices[eLine] = pNeighbor->m_iIndex;
	}

public:
	HRESULT		Initialize(const _float3* pPoints, _int iIndex, std::string cellKey);
	HRESULT		Render();

	_bool		Is_In(_fvector vPosition, _int* pNeighborIndex);	// 이동용
	_bool		Is_In(_fvector vPosition, _int* pNeighborIndex, _int* pHitEdgeIndex);	// 슬라이딩 벡터용
	_bool		Compare_Points(_fvector vSourPoint, _fvector vDestPoint);

	_float		Compute_Height(_fvector vPosition);

	_bool		Is_Picked(_fvector& localMousePos, _fvector& localMouseRay, _float& outDist);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	_float3						m_vPoints[POINT_END] = {};
	_float3						m_vNormals[LINE_END] = {};
	_int						m_iIndex = {};
	_int						m_iNeighborCellIndices[LINE_END] = { -1, -1, -1 };

	_float4						m_vPlane = {};

	std::string					m_CellKey = {};

#ifdef _DEBUG
	class CVIBuffer_Cell* m_pVIBuffer = { nullptr };
#endif

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex, std::string cellKey);
	void	Free()	override;
};

END