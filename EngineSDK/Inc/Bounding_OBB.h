#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_OBB final : public CBounding
{
public:
	typedef struct tagBoundingOBBDesc : public CBounding::BOUNDING_DESC
	{
		_float3		vExtents;
		_float3		vRotation;

	}BOUNDING_OBB_DESC;

private:
	CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	~CBounding_OBB() override = default;

public:
	void* Get_Desc() override { return m_pDesc; }

	void	Edit_Bounding_Center(_float3 moveCenter) const
	{
		m_pLocalDesc->Center.x += moveCenter.x;
		m_pLocalDesc->Center.y += moveCenter.y;
		m_pLocalDesc->Center.z += moveCenter.z;
	}

	void	Edit_Bounding_Extent(_float3 editExtent) const
	{
		m_pLocalDesc->Extents.x += editExtent.x;
		m_pLocalDesc->Extents.y += editExtent.y;
		m_pLocalDesc->Extents.z += editExtent.z;
	}

public:
	HRESULT		Initialize(const BOUNDING_DESC* pArg);
	void		Update(_fmatrix WorldMatrix)	override;
	_bool		Intersect(COLLIDER eColliderType, CBounding* pTargetBounding, void* pRayArg = nullptr)	override;

#ifdef _DEBUG
	HRESULT		Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) override;
#endif

private:
	BoundingOrientedBox*	m_pLocalDesc = { nullptr };
	BoundingOrientedBox*	m_pDesc = { nullptr };

public:
	static CBounding_OBB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const BOUNDING_DESC* pArg);	
	void	Free() override;
};

END