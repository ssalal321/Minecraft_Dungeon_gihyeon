#include "Bounding_Sphere.h"

#include "DebugDraw.h"

CBounding_Sphere::CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding{ pDevice, pContext }
{
}

HRESULT CBounding_Sphere::Initialize(const BOUNDING_DESC* pArg)
{
	const BOUNDING_SPHERE_DESC* pDesc = static_cast<const BOUNDING_SPHERE_DESC*>(pArg);

	m_pLocalDesc = new BoundingSphere(pDesc->vCenter, pDesc->fRadius);
	m_pDesc = new BoundingSphere(*m_pLocalDesc);

	return S_OK;
}

void CBounding_Sphere::Update(_fmatrix WorldMatrix)
{	
	m_pLocalDesc->Transform(*m_pDesc, WorldMatrix);
}

_bool CBounding_Sphere::Intersect(COLLIDER_TYPE eColliderType, CBounding* pTargetBounding, void* pArg)
{
	void* pTargetDesc = nullptr;
	if (pTargetBounding)
		pTargetDesc = pTargetBounding->Get_Desc();

	_bool		isColl = { false };

	switch (eColliderType)
	{
	case COLLIDER_TYPE::TYPE_AABB:
		isColl = m_pDesc->Intersects(*static_cast<BoundingBox*>(pTargetDesc));
		break;
	case COLLIDER_TYPE::TYPE_OBB:
		isColl = m_pDesc->Intersects(*static_cast<BoundingOrientedBox*>(pTargetDesc));
		break;
	case COLLIDER_TYPE::TYPE_SPHERE:
		isColl = m_pDesc->Intersects(*static_cast<BoundingSphere*>(pTargetDesc));
		break;

	case COLLIDER_TYPE::TYPE_RAY:
		{
			RayDesc*	pRayDesc = static_cast<RayDesc*>(pArg);
			if (nullptr == pRayDesc)
				return false;

			_fvector	vMousePos = XMLoadFloat3(&pRayDesc->MousePos);
			_fvector	vMouseRay = XMLoadFloat3(&pRayDesc->MouseRay);
			isColl = m_pDesc->Intersects(vMousePos, vMouseRay, *pRayDesc->fDist);
		}
		break;
	}

	return isColl;
}

HRESULT CBounding_Sphere::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
	pBatch->Begin();

	DX::Draw(pBatch, *m_pDesc, vColor);

	pBatch->End();

	return S_OK;
}

CBounding_Sphere* CBounding_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const BOUNDING_DESC* pArg)
{
	CBounding_Sphere* pGameInstance = new CBounding_Sphere(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : CBounding_Sphere");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


void CBounding_Sphere::Free()
{
	__super::Free();

	Safe_Delete(m_pLocalDesc);
	Safe_Delete(m_pDesc);
}
