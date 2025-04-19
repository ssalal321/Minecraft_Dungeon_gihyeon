#pragma once

#include "Base.h"

BEGIN(Engine)
class CGameObject;

class CBounding abstract : public CBase
{
public:
	typedef struct tagBoundingDesc
	{
		_float3			vCenter;
		_float4x4*		CombinedWorldMatrix;
		CGameObject*	pGameObject;

		_bool*	pContainerObjAttacking = { nullptr };

	}BOUNDING_DESC;

protected:
	CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	~CBounding() override = default;

public:
	virtual  void*	Get_Desc() = 0;

public:
	virtual  void	Update(_fmatrix WorldMatrix) = 0;
	virtual  _bool	Intersect(COLLIDER eColliderType, CBounding* pTargetBounding) = 0;

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) = 0;
#endif

protected:
	ID3D11Device*			m_pDevice  = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

public:
	void Free() override;
};

END