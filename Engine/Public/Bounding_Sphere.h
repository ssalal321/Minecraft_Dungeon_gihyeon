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

public:
	HRESULT		Initialize(const BOUNDING_DESC* pArg);
	void		Update(_fmatrix WorldMatrix)	override;
	_bool		Intersect(COLLIDER eColliderType, CBounding* pTargetBounding, void* pRayArg = nullptr)	override;

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