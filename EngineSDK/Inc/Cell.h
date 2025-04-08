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
	_vector Get_Point(POINT ePoint) {
		return XMLoadFloat3(&m_vPoints[ePoint]);
	}

	void SetUp_Neighbor(LINE eLine, CCell* pNeighbor) {
		m_iNeighborCellIndices[eLine] = pNeighbor->m_iIndex;
	}

public:
	HRESULT Initialize(const _float3* pPoints, _int iIndex);
	HRESULT Render();
	
	_bool is_In(_fvector vPosition, _int* pNeighborIndex);
	_bool Compare_Points(_fvector vSourPoint, _fvector vDestPoint);

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	_float3						m_vPoints[POINT_END] = {};
	_float3						m_vNormals[LINE_END] = {};
	_int						m_iIndex = {};
	_int						m_iNeighborCellIndices[LINE_END] = { -1, -1, -1 };

#ifdef _DEBUG
	class CVIBuffer_Cell* m_pVIBuffer = { nullptr };
#endif

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex);
	void	Free()	override;
};

END