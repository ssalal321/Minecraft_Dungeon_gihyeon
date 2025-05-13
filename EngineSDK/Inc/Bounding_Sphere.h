#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_Sphere final : public CBounding
{
public:
	typedef struct tagBoundingSphereDesc : public CBounding::BOUNDING_DESC
	{
		_float		fRadius;

	}BOUNDING_SPHERE_DESC;

	struct RayDesc
	{
		_float3		MousePos = { 0.f, 0.f, 0.f };
		_float3		MouseRay = { 0.f, 0.f, 0.f };
		_float*		fDist = {};
	};

private:
	CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	~CBounding_Sphere() override = default;

public:
	void*		Get_Desc() override { return m_pDesc; }

	_float		Get_Radius() const { return m_pLocalDesc->Radius; }

	void	Edit_Bounding_Center(_float3 moveCenter) const
	{
		m_pLocalDesc->Center.x += moveCenter.x;
		m_pLocalDesc->Center.y += moveCenter.y;
		m_pLocalDesc->Center.z += moveCenter.z;
	}

	void	Edit_Bounding_Radius(_float editRadius) const
	{
		m_pLocalDesc->Radius += editRadius;
	}

	void	ReSet_Bounding_Center(_float3 center) const
	{
		m_pLocalDesc->Center.x = center.x;
		m_pLocalDesc->Center.y = center.y;
		m_pLocalDesc->Center.z = center.z;
	}

	void	ReSet_Bounding_Radius(_float fRadius) const
	{
		m_pLocalDesc->Radius = fRadius;
	}

public:
	HRESULT		Initialize(const BOUNDING_DESC* pArg);
	void		Update(_fmatrix WorldMatrix)	override;
	_bool		Intersect(COLLIDER_TYPE eColliderType, CBounding* pTargetBounding, void* pRayArg = nullptr)	override;

#ifdef _DEBUG
	HRESULT		Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) override;
#endif

private:
	BoundingSphere*		m_pLocalDesc = { nullptr };
	BoundingSphere*		m_pDesc = { nullptr };

public:
	static	CBounding_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const BOUNDING_DESC* pArg);	
	void	Free()	override;
};

END