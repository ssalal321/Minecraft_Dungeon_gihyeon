#include "Collision_Manager.h"

#include <iostream>
#include <ostream>

#include "ContainerObject.h"
#include "GameInstance.h"
#include "PartObject.h"

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
    if (!pColliderComponent)
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

    // 1. Collider Update
    for (auto& group : colliderGroups)
    {
        for (auto& collider : group.second)
        {
            if (collider)
                collider->Update();
        }
    }

    // 2. 서로 다른 그룹 간 충돌 검사
    for (auto iterA = colliderGroups.begin(); iterA != colliderGroups.end(); ++iterA)
    {
        auto& groupA = iterA->second;

        auto iterB = iterA;
        ++iterB;

        for (; iterB != colliderGroups.end(); ++iterB)
        {
            auto& groupB = iterB->second;

            for (auto* pColliderA : groupA)
            {
                if (!pColliderA || pColliderA->Is_MouseCollider()) continue;

                for (auto* pColliderB : groupB)
                {
                    if (!pColliderB || pColliderB->Is_MouseCollider()) continue;

                    if (pColliderA->Intersect(pColliderB))
                    {
                        pColliderA->Collided_With(pColliderB);
                        pColliderB->Collided_With(pColliderA);

                        // 여기서 슬라이딩 처리
                        // SMALL vs SMALL 충돌 시 자동으로 밀어내기 적용
                        if (pColliderA->Get_Role() == CCollider::SMALL && pColliderB->Get_Role() == CCollider::SMALL 
                            && pColliderA < pColliderB)
                        {
                            Resolve_Penetration_And_Slide(pColliderA, pColliderB, 9.f);
                        }
                    }
                }
            }
        }
    }

    // 3. 동일 그룹 내 조건부 충돌 검사 (AllowInGroupCollision 활성화된 경우만)
    for (auto& group : colliderGroups)
    {
        auto& colliders = group.second;

        for (size_t i = 0; i < colliders.size(); ++i)
        {
            CCollider* pColliderA = colliders[i];
            if (!pColliderA || !pColliderA->Get_AllowSameGroupCollision() || pColliderA->Is_MouseCollider()) continue;

            for (size_t j = i + 1; j < colliders.size(); ++j)
            {
                CCollider* pColliderB = colliders[j];
                if (!pColliderB || !pColliderB->Get_AllowSameGroupCollision() || pColliderB->Is_MouseCollider()) continue;

                if (pColliderA->Intersect(pColliderB))
                {
                    pColliderA->Collided_With(pColliderB);
                    pColliderB->Collided_With(pColliderA);

                    // 여기서 슬라이딩 처리
                    // SMALL vs SMALL 충돌 시 자동으로 밀어내기 적용
                    if (pColliderA->Get_Role() == CCollider::SMALL && pColliderB->Get_Role() == CCollider::SMALL
                        && pColliderA < pColliderB)
                    {
                        Resolve_Penetration_And_Slide(pColliderA, pColliderB, 9.f);
                    }
                }
            }
        }
    }

    // 4. 충돌 상태 정리 (Enter / Stay / Exit 처리)
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


void CCollision_Manager::Resolve_Penetration_And_Slide(CCollider* pColA, CCollider* pColB, _float fForce)
{
    if (!pColA || !pColB || !pColA->Is_ColliderActive() || !pColB->Is_ColliderActive())
        return;

    CPartObject*    pPartObjA = dynamic_cast<CPartObject*>(pColA->Get_OwnerObject());
    CPartObject*    pPartObjB = dynamic_cast<CPartObject*>(pColB->Get_OwnerObject());
    CContainerObject*   pContainerObjA = pPartObjA->Get_ContainerObject();
    CContainerObject*   pContainerObjB = pPartObjB->Get_ContainerObject();
        

    if (!pContainerObjA || !pContainerObjB)
        return;

    CTransform*     pA_TransformCom = dynamic_cast<CTransform*>(pContainerObjA->Find_Component(TEXT("Com_Transform")));
    CTransform*     pB_TransformCom = dynamic_cast<CTransform*>(pContainerObjB->Find_Component(TEXT("Com_Transform")));

    if (!pA_TransformCom || !pB_TransformCom)
        return;

    // 두 Sphere의 중심점 계산
    _vector     vPositionA = pA_TransformCom->Get_State(CTransform::STATE_POSITION);
    _vector     vPositionB = pB_TransformCom->Get_State(CTransform::STATE_POSITION);

    // 충돌 방향 벡터 계산
    _vector     vAtoB  = vPositionB - vPositionA;
    _float      fLen    = XMVectorGetX(XMVector3Length(vAtoB));
    if (fLen < 0.0001f)
        return;

    _vector  vNormalizedAtoB = XMVector3Normalize(vAtoB);


    _float fDistance = XMVectorGetX(XMVector3Length(vPositionB - vPositionA));
    _float fA_Radius = dynamic_cast<CBounding_Sphere*>(pColA->Get_Bounding())->Get_Radius();
    _float fB_Radius = dynamic_cast<CBounding_Sphere*>(pColB->Get_Bounding())->Get_Radius();

    _float fPenetration = (fA_Radius + fB_Radius) - fDistance;
    if (fPenetration <= 0.f)
        return;

    _float ratioA = fB_Radius / (fA_Radius + fB_Radius);
    _float ratioB = fA_Radius / (fA_Radius + fB_Radius);

    // 강제 밀어내는 양
    _float fPushForce = min(fPenetration * fForce, 5.f);

    // [ 슬라이딩 방향 계산 - 객체 A의 Look 기준 ]
    _vector vLookA = pA_TransformCom->Get_State(CTransform::STATE_LOOK);
    vLookA = XMVector3Normalize(vLookA);

    //// 접선 방향 = LookA - (LookA ? Normal) * Normal
    //_vector vSlideA = XMVector3Normalize(vLookA - XMVectorScale( vNormalizedAtoB, XMVectorGetX(XMVector3Dot(vLookA,  vNormalizedAtoB))));

    //// 최종 슬라이딩 벡터
    //_vector vSlidePushA = vSlideA * fPushForce * ratioA;

    // 기본 반발 밀어내기 벡터
    _vector vBouncePushA = - vNormalizedAtoB * fPushForce * ratioA;
    _vector vBouncePushB = + vNormalizedAtoB * fPushForce * ratioB;

    // 슬라이딩 + 반발 벡터 조합
    _vector vFinalPushA = vBouncePushA;/*XMVectorLerp(, vSlidePushA, 0.7f);*/  // 슬라이딩 위주
    _vector vFinalPushB = vBouncePushB; // 그냥 밀리기만

    if (!pContainerObjA->Get_Stationary())
		pContainerObjA->Apply_Penetration_Momentum(vFinalPushA);

    if (!pContainerObjB->Get_Stationary())
		pContainerObjB->Apply_Penetration_Momentum(vFinalPushB);
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
    /*if (m_pPersistentColliderGroups)
    {
        for (auto& pair : *m_pPersistentColliderGroups)
            for (auto* pCol : pair.second)
                Safe_Release(pCol);

        m_pPersistentColliderGroups->clear();
    }*/

    Safe_Delete_Array(m_pColliderGroups);
    Safe_Delete(m_pPersistentColliderGroups);

    Safe_Release(m_pGameInstance);
    __super::Free();
}

