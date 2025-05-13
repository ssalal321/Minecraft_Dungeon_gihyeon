#include "Bounding_OBB.h"

#include "DebugDraw.h"

CBounding_OBB::CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding{ pDevice, pContext }
{
}

HRESULT CBounding_OBB::Initialize(const BOUNDING_DESC* pArg)
{
	const BOUNDING_OBB_DESC* pDesc = static_cast<const BOUNDING_OBB_DESC*>(pArg);

	_float4		vQuaternion = {};
	XMStoreFloat4(&vQuaternion, XMQuaternionRotationRollPitchYaw(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z));

	m_pLocalDesc = new BoundingOrientedBox(pDesc->vCenter, pDesc->vExtents, vQuaternion);
	m_pDesc = new BoundingOrientedBox(*m_pLocalDesc);

	return S_OK;
}

void CBounding_OBB::Update(_fmatrix WorldMatrix)
{	
	m_pLocalDesc->Transform(*m_pDesc, WorldMatrix);
}

_bool CBounding_OBB::Intersect(COLLIDER_TYPE eColliderType, CBounding* pTargetBounding, void* pRayArg)
{
	void* pTargetDesc = pTargetBounding->Get_Desc();

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
	}

	return isColl;
}

HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
	pBatch->Begin();

	DX::Draw(pBatch, *m_pDesc, vColor);

	pBatch->End();

	return S_OK;
}

CBounding_OBB* CBounding_OBB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const BOUNDING_DESC* pArg)
{
	CBounding_OBB* pGameInstance = new CBounding_OBB(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : CBounding_OBB");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


void CBounding_OBB::Free()
{
	__super::Free();

	Safe_Delete(m_pLocalDesc);
	Safe_Delete(m_pDesc);
}
