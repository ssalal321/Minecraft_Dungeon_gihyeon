#include "Bounding_AABB.h"

#include "DebugDraw.h"

CBounding_AABB::CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding{ pDevice, pContext }
{
}

HRESULT CBounding_AABB::Initialize(const BOUNDING_DESC* pArg)
{
	const BOUNDING_AABB_DESC* pDesc = static_cast<const BOUNDING_AABB_DESC*>(pArg);

	m_pLocalDesc = new BoundingBox(pDesc->vCenter, pDesc->vExtents);
	m_pDesc = new BoundingBox(*m_pLocalDesc);
	
	return S_OK;
}

void CBounding_AABB::Update(_fmatrix WorldMatrix)
{
	_matrix		TransformMatrix = WorldMatrix;

	TransformMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(TransformMatrix.r[2]);

	m_pLocalDesc->Transform(*m_pDesc, TransformMatrix);
}

_bool CBounding_AABB::Intersect(COLLIDER eColliderType, CBounding* pTargetBounding)
{
	void* pTargetDesc = pTargetBounding->Get_Desc();

	_bool		isColl = { false };

	switch (eColliderType)
	{
	case COLLIDER::TYPE_AABB:
		isColl = m_pDesc->Intersects(*static_cast<BoundingBox*>(pTargetDesc));
		break;
	case COLLIDER::TYPE_OBB:
		isColl = m_pDesc->Intersects(*static_cast<BoundingOrientedBox*>(pTargetDesc));
		break;
	case COLLIDER::TYPE_SPHERE:
		isColl = m_pDesc->Intersects(*static_cast<BoundingSphere*>(pTargetDesc));
		break;
	}

	return isColl;
}

HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
	pBatch->Begin();

	DX::Draw(pBatch, *m_pDesc, vColor);

	pBatch->End();

	return S_OK;
}

CBounding_AABB* CBounding_AABB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const BOUNDING_DESC* pArg)
{
	CBounding_AABB* pGameInstance = new CBounding_AABB(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : CBounding_AABB");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


void CBounding_AABB::Free()
{
	__super::Free();

	Safe_Delete(m_pLocalDesc);
	Safe_Delete(m_pDesc);
}
