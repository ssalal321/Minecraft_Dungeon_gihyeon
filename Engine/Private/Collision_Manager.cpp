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

HRESULT CCollision_Manager::Add_ColliderCom(CComponent* pCollider, const _wstring& PartObject_Tag, const _wstring& ContainerGroup_Tag)
{
	CCollider* pColliderCom = dynamic_cast<CCollider*>(pCollider);

	if (!pCollider)
		return E_FAIL;

	pColliderCom->Set_OwnerTag(PartObject_Tag);

	m_ColliderGroups[ContainerGroup_Tag].emplace_back(pColliderCom);
	return S_OK;
}

void CCollision_Manager::Update()
{
    // 1. 각 Collider의 Update 호출 (월드 행렬 등 갱신용)
    for (auto& group : m_ColliderGroups)
    {
        for (auto* collider : group.second)
        {
            if (collider)
                collider->Update();
        }
    }

    // 2. 그룹 간 충돌 검사 (중복 제거)
    for (auto ownerTypeA_iter = m_ColliderGroups.begin(); ownerTypeA_iter != m_ColliderGroups.end(); ++ownerTypeA_iter)
    {
        vector<CCollider*>& collidersInGroupA = ownerTypeA_iter->second;

        auto ownerTypeB_iter = ownerTypeA_iter;
        ++ownerTypeB_iter;

        for (; ownerTypeB_iter != m_ColliderGroups.end(); ++ownerTypeB_iter)
        {
            vector<CCollider*>& collidersInGroupB = ownerTypeB_iter->second;

            for (auto* colliderA : collidersInGroupA)
            {
                if (!colliderA) continue;

                for (auto* colliderB : collidersInGroupB)
                {
                    if (!colliderB) continue;

                    if (colliderA->Intersect(colliderB))
                    {
                        // 충돌 쌍을 서로에게 기록
                        colliderA->Collided_With(colliderB);
                        colliderB->Collided_With(colliderA);
                    }
                }
            }
        }
    }

    // 3. 모든 Collider의 충돌 상태 정리 (Enter, Stay, Exit 판단)
    for (auto& group : m_ColliderGroups)
    {
        for (auto* collider : group.second)
        {
            if (collider)
                collider->Process_Collisions();
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
