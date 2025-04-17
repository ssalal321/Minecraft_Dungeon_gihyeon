#include "Collision_Manager.h"
#include "GameInstance.h"

CCollision_Manager::CCollision_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCollision_Manager::Initialize(_uint iNumLevels)
{
	m_iNumLevels = iNumLevels;
	return S_OK;
}

HRESULT CCollision_Manager::Add_ColliderCom(CComponent* pCollider, const std::wstring& ownerTypeTag)
{
	CCollider* pColliderCom = dynamic_cast<CCollider*>(pCollider);

	if (!pCollider)
		return E_FAIL;

	pColliderCom->Set_OwnerTag(ownerTypeTag);

	m_ColliderGroups[ownerTypeTag].emplace_back(pColliderCom);
	return S_OK;
}

void CCollision_Manager::Update()
{
	// m_ColliderGroups의 Collider 각각 update 호출
	for (auto& group : m_ColliderGroups)
	{
		for (auto* collider : group.second)
		{
			if (collider)
				collider->Update();  // CombinedWorldMatrix
		}
	}

	// 그룹 간 충돌 검사
	for (auto ownerTypeA_iter = m_ColliderGroups.begin(); ownerTypeA_iter != m_ColliderGroups.end(); ++ownerTypeA_iter)
	{
		// ownerA_iter : 그룹 A
		vector<CCollider*>& collidersInGroupA = ownerTypeA_iter->second;	// 그룹 A의 콜라이더들

		for (auto ownerTypeB_iter = m_ColliderGroups.begin(); ownerTypeB_iter != m_ColliderGroups.end(); ++ownerTypeB_iter)
		{
			if (ownerTypeA_iter == ownerTypeB_iter)	// (그룹 A == 그룹 B) : 같은 타입 내 충돌 연산 X
				continue;

			// ownerB_iter : 그룹 B
			vector<CCollider*>& collidersInGroupB = ownerTypeB_iter->second;	// 그룹 B의 콜라이더들

			for (auto* colliderA : collidersInGroupA)
			{
				if (!colliderA) continue;					// nullptr 검사
				for (auto* colliderB : collidersInGroupB)
				{
					if (!colliderB) continue;				// nullptr 검사
					if (colliderA->Intersect(colliderB))
					{
						colliderA->Is_Hit(colliderB);
						colliderB->Is_Hit(colliderA);
					}
				}
			}
		}
	}
}


#ifdef _DEBUG
HRESULT CCollision_Manager::Render()
{
	for (const auto& pair : m_ColliderGroups)
	{
		const std::wstring& ownerType = pair.first;
		const std::vector<CCollider*>& colliderGroup = pair.second;

		for (auto* pCollider : colliderGroup)
		{
			if (pCollider)
				pCollider->Render();
		}
	}
	return S_OK;
}
#endif


void CCollision_Manager::Clear(_uint currentLevelIndex)
{
	if (currentLevelIndex >= m_iNumLevels)
		return;

	for (auto& pair : m_ColliderGroups)
	{
		std::vector<CCollider*>& colliderGroup = pair.second;

		for (auto* pCollider : colliderGroup)
		{
			Safe_Release(pCollider);
		}
	}

	m_ColliderGroups.clear();
}


CCollision_Manager* CCollision_Manager::Create(_uint iNumLevels)
{
	CCollision_Manager* pInstance = new CCollision_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Create : CCollision_Manager");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCollision_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
