#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_AABB final : public CBounding
{
public:
	typedef struct tagBoundingAABBDesc : public CBounding::BOUNDING_DESC
	{
		_float3		vExtents;

	}BOUNDING_AABB_DESC;

private:
	CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	~CBounding_AABB() override = default;

public:
	void*		Get_Desc() override { return m_pDesc; }

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
	HRESULT		Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)	override;
#endif

private:
	BoundingBox*	m_pLocalDesc	= { nullptr };
	BoundingBox*	m_pDesc			= { nullptr };

public:
	static	CBounding_AABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const BOUNDING_DESC* pArg);
	void	Free()	override;
};

END