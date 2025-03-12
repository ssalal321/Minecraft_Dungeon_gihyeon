#pragma once

#include "Base.h"

BEGIN(Engine)

class CPipeLine final : public CBase
{
public:
	enum TRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };
private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	const _float4x4* Get_Transform_Float4x4(TRANSFORMSTATE eState) const {
		return &m_TransformationMatrix[eState];
	}

	const _matrix Get_Transform_Matrix(TRANSFORMSTATE eState) const {
		return XMLoadFloat4x4(&m_TransformationMatrix[eState]);
	}

	const _float4x4* Get_Transform_Inverse_Float4x4(TRANSFORMSTATE eState) const {
		return &m_TransformationMatrix_Inverse[eState];
	}

	const _matrix Get_Transform_Inverse_Matrix(TRANSFORMSTATE eState) const {
		return XMLoadFloat4x4(&m_TransformationMatrix_Inverse[eState]);
	}

	const _float4* Get_CamPosition() const {
		return &m_vCamPosition;
	}

public:
	void Set_Transform(TRANSFORMSTATE eState, _fmatrix StateMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix[eState], StateMatrix);
	}
	

public:
	void Update();

private:
	_float4x4				m_TransformationMatrix[D3DTS_END] = {};
	_float4x4				m_TransformationMatrix_Inverse[D3DTS_END] = {};
	_float4					m_vCamPosition = {};

public:
	static CPipeLine* Create();
	virtual void Free();

};

END