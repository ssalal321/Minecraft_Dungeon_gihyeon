#include "Collision_Manager.h"

#include <iostream>
#include <ostream>

#include "GameInstance.h"
#include "GameObject.h"

CCollision_Manager::CCollision_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCollision_Manager::Initialize(_uint iNumLevels)
{
    m_iNumLevels = iNumLevels;

    // 일반 레벨별 Collider 그룹 초기화
    m_pColliderGroups = new unordered_map<wstring, vector<CCollider*>>[iNumLevels];

    // Persistent Collider 그룹 초기화
    m_pPersistentColliderGroups = new unordered_map<wstring, vector<CCollider*>>;

    return S_OK;
}

HRESULT CCollision_Manager::Add_ColliderCom(_uint iLevelIndex, CComponent* pColliderCom, const _wstring& ColliderTag, const _wstring& ObjectType, _bool bPersistent)
{
    if (!pColliderCom)
        return E_FAIL;

    // Collider로 캐스팅
    CCollider* pColliderComponent = dynamic_cast<CCollider*>(pColliderCom);
    if (!pColliderCom)
        return E_FAIL;

    pColliderComponent->Set_ColliderTag(ColliderTag);

    // Persistent이면 별도 저장
    if (bPersistent && m_pPersistentColliderGroups)
    {
        (*m_pPersistentColliderGroups)[ObjectType].emplace_back(pColliderComponent);
        m_PersistentTags.insert(ObjectType);
    }

    m_pColliderGroups[iLevelIndex][ObjectType].emplace_back(pColliderComponent);

    Safe_AddRef(pColliderComponent);
    return S_OK;
}

HRESULT CCollision_Manager::Attach_Persistent_Colliders_To_Level(_uint iLevelIndex, const wstring& targetTag)
{
    if (!m_pPersistentColliderGroups || iLevelIndex >= m_iNumLevels)
        return E_FAIL;

    // 특정 태그만 찾아서 붙임
    auto iter = m_pPersistentColliderGroups->find(targetTag);
    if (iter == m_pPersistentColliderGroups->end())
        return E_FAIL;  // 해당 tag 없음

    auto& persistentGroup = iter->second;
    auto& dstGroup = m_pColliderGroups[iLevelIndex][targetTag];

    for (auto* pCol : persistentGroup)
    {
        if (find(dstGroup.begin(), dstGroup.end(), pCol) == dstGroup.end())
        {
            dstGroup.push_back(pCol);
            Safe_AddRef(pCol); // 필요시 주석 해제
        }
    }

    return S_OK;
}



unordered_map<_wstring, vector<CCollider*>>* CCollision_Manager::Get_Colliders(_uint iLevelIndex)
{
    if (iLevelIndex >= m_iNumLevels || m_pColliderGroups == nullptr)
        return nullptr;

    return &m_pColliderGroups[iLevelIndex];
}


void CCollision_Manager::Update()
{
    _uint currentLevel = m_pGameInstance->Get_CurrentLevelIndex();
    auto& colliderGroups = m_pColliderGroups[currentLevel];

    // 1. 각 Collider의 Update 호출 (월드 행렬 등 갱신용)
    for (auto& group : colliderGroups)
    {
        for (auto* collider : group.second)
        {
            if (collider)
                collider->Update();
        }
    }

    // 2. 그룹 간 충돌 검사 (중복 제거)
    for (auto ownerTypeA_iter = colliderGroups.begin(); ownerTypeA_iter != colliderGroups.end(); ++ownerTypeA_iter)
    {
        vector<CCollider*>& collidersInGroupA = ownerTypeA_iter->second;

        auto ownerTypeB_iter = ownerTypeA_iter;
        ++ownerTypeB_iter;

        for (; ownerTypeB_iter != colliderGroups.end(); ++ownerTypeB_iter)
        {
            vector<CCollider*>& collidersInGroupB = ownerTypeB_iter->second;

            for (auto* colliderA : collidersInGroupA)
            {
                if (!colliderA || colliderA->Get_MouseCollider()) continue;   // null 검사

                for (auto* colliderB : collidersInGroupB)
                {
                    if (!colliderB || colliderB->Get_MouseCollider()) continue;  // null 검사

                    if (colliderA->Intersect(colliderB))
                    {
                        // 충돌 쌍을 서로에게 기록
                        colliderA->Collided_With(colliderB);
                        colliderB->Collided_With(colliderA);

                        /*std::wcerr << "[" << colliderA->Get_OwnerObject()->Get_GameObjectTag() << "]와 ["
                    	<< colliderB->Get_OwnerObject()->Get_GameObjectTag() << "] 충돌" << std::endl;*/
                    }
                }
            }
        }
    }

    // 3. 모든 Collider의 충돌 상태 정리 (Enter, Stay, Exit 판단)
    for (auto& group : colliderGroups)
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
    _uint currentLevel = m_pGameInstance->Get_CurrentLevelIndex();
    auto& colliderGroups = m_pColliderGroups[currentLevel];

	for (const auto& pair : colliderGroups)
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


void CCollision_Manager::Clear(_uint iLevelIndex)
{
    if (iLevelIndex >= m_iNumLevels)
        return;

    auto& colliderGroups = m_pColliderGroups[iLevelIndex];

    // 각 그룹을 순회하면서
    for (auto it = colliderGroups.begin(); it != colliderGroups.end();)
    {
        const wstring& tag = it->first;

        // Persistent로 설정된 태그를 가진 Collider는 제거하지 않고 건너뜀
        // 추후 다시 레벨로 돌아왔을 때 다시 추가하지 않아도 됨
        if (m_PersistentTags.find(tag) != m_PersistentTags.end())
        {
            ++it;
            continue;
        }

        // 일반 Collider는 해제
        for (auto* pCol : it->second)
            Safe_Release(pCol);

        it = colliderGroups.erase(it);
    }

    // Persistent Collider들의 충돌 정보 초기화
    for (const auto& tag : m_PersistentTags)
    {
        auto iter = m_pPersistentColliderGroups->find(tag);
        if (iter == m_pPersistentColliderGroups->end())
            continue;

        for (auto* pCollider : iter->second)
        {
            if (pCollider)
            {
                pCollider->Clear_Collision_State(); // 충돌 상태 초기화
            }
        }
    }

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
    // 레벨별 Collider 해제
    for (_uint i = 0; i < m_iNumLevels; ++i)
    {
        for (auto& pair : m_pColliderGroups[i])
            for (auto* pCollider : pair.second)
                Safe_Release(pCollider);

        m_pColliderGroups[i].clear();
    }

    // Persistent Collider 해제
    if (m_pPersistentColliderGroups)
    {
        for (auto& pair : *m_pPersistentColliderGroups)
            for (auto* pCol : pair.second)
                Safe_Release(pCol);

        m_pPersistentColliderGroups->clear();
    }

    Safe_Delete_Array(m_pColliderGroups);
    Safe_Delete(m_pPersistentColliderGroups);

    Safe_Release(m_pGameInstance);
    __super::Free();
}

