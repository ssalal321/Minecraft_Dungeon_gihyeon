#include "Collider.h"

#include <iostream>

#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

#include "GameInstance.h"
#include "GameObject.h"


CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent( pDevice, pContext )
{
}

CCollider::CCollider(const CCollider& Prototype)
	: CComponent( Prototype )
	, m_eColliderType{ Prototype.m_eColliderType }
#ifdef _DEBUG
	, m_pBatch{ Prototype.m_pBatch }
	, m_pEffect{ Prototype.m_pEffect }
	, m_pInputLayout{ Prototype.m_pInputLayout }
#endif
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif
}

HRESULT CCollider::Initialize_Prototype(COLLIDER_TYPE eColliderType)
{
	m_eColliderType = eColliderType;

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);

	const void* pShaderByteCode = { nullptr };
	size_t	iShaderCodeLength = {};

	m_pEffect->SetVertexColorEnabled(true);

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderCodeLength, &m_pInputLayout)))
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
	CBounding::BOUNDING_DESC* pDesc = static_cast<CBounding::BOUNDING_DESC*>(pArg);

	switch (m_eColliderType)
	{
	case COLLIDER_TYPE::TYPE_AABB:
		m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, pDesc);
		break;
	case COLLIDER_TYPE::TYPE_OBB:
		m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext, pDesc);
		break;
	case COLLIDER_TYPE::TYPE_SPHERE:
		m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext, pDesc);
		break;
	}

	m_CombinedWorldMatrix	 = pDesc->CombinedWorldMatrix;
	m_pOwnerGameObject		 = pDesc->pGameObject;
	m_bCollisionActivated	 = pDesc->pCollisionActivated;

	return S_OK;
}

_bool CCollider::Intersect(CCollider* pTargetCollider)
{
	if (false == *m_bCollisionActivated && false == pTargetCollider->Get_Other_Collision_Activated() || !m_bColliderActive)
	{
		m_bIsCollision = false;
		pTargetCollider->Set_IsCollision(false);
		return false;
	}
		
	m_bIsCollision = m_pBounding->Intersect(pTargetCollider->m_eColliderType, pTargetCollider->m_pBounding);

	pTargetCollider->Set_IsCollision(m_bIsCollision);

	return m_bIsCollision;
}

void CCollider::Collided_With(CCollider* pOther)
{
	m_currCollisions.insert(pOther);
}

void CCollider::Process_Collisions()
{
	// Enter or Stay
	for (auto* pOther : m_currCollisions)
	{
		if (m_prevCollisions.find(pOther) != m_prevCollisions.end())
		{
			if (nullptr == m_pOwnerGameObject)
				return;

			m_pOwnerGameObject->Collided_With(pOther, STAY);

			/*std::wcerr << "[" << m_pOwnerGameObject->Get_GameObjectTag() << "]客 ["
				<< pOther->Get_OwnerObject()->Get_GameObjectTag() << "] 面倒 Stay" << std::endl;*/
		}

		else
		{
			if (nullptr == m_pOwnerGameObject)
				return;

			m_pOwnerGameObject->Collided_With(pOther, ENTER);

			/*std::wcerr << "[" << m_pOwnerGameObject->Get_GameObjectTag() << "]客 ["
				<< pOther->Get_OwnerObject()->Get_GameObjectTag() << "] 面倒 Enter" << std::endl;*/
		}
	}

	// Exit
	for (auto* pOther : m_prevCollisions)
	{
		if (m_currCollisions.find(pOther) == m_currCollisions.end())
		{
			m_pOwnerGameObject->Collided_With(pOther, EXIT);

			/*std::wcerr << "[" << m_pOwnerGameObject->Get_GameObjectTag() << "]客 ["
				<< pOther->Get_OwnerObject()->Get_GameObjectTag() << "] 面倒 Exit" << std::endl;*/
		}
	}

	// Prepare for next frame
	m_prevCollisions = std::move(m_currCollisions);
	m_currCollisions.clear();
}


void CCollider::Update()
{
	if (!m_bColliderActive)
		return;

	_matrix  mCombinedWorldMatrix = XMLoadFloat4x4(m_CombinedWorldMatrix);
	m_pBounding->Update(mCombinedWorldMatrix);
}

#ifdef _DEBUG
HRESULT CCollider::Render()
{
	if (!m_bColliderActive)
		return S_OK;

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pEffect->Apply(m_pContext);


	m_pBounding->Render(m_pBatch, true == m_bIsCollision ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	return S_OK;
}
#endif

void CCollider::Clear_Collision_State()
{
	m_prevCollisions.clear();
	m_currCollisions.clear();
}

CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER_TYPE eColliderType)
{
	CCollider* pGameInstance = new CCollider(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype(eColliderType)))
	{
		MSG_BOX("Failed to Create : CCollider");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CComponent* CCollider::Clone(void* pArg)
{
	CCollider* pGameInstance = new CCollider(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CCollider");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CCollider::Free()
{
	__super::Free();

	Safe_Release(m_pBounding);

#ifdef _DEBUG
	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
	Safe_Release(m_pInputLayout);

#endif
}
