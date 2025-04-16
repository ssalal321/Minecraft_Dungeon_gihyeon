#include "Collision_Manager.h"
#include "GameInstance.h"

CCollision_Manager::CCollision_Manager() : m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCollision_Manager::Initialize(_uint iNumLevels)
{
	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CCollision_Manager::Add_ColliderCom(CComponent* pColliderCom/*, _wstring OwnerSortTag*/)
{
	if (nullptr == pColliderCom)
		return E_FAIL;

	CCollider* CollCom = dynamic_cast<CCollider*>(pColliderCom);
	m_pColliders.push_back(CollCom);

	return S_OK;
}


void CCollision_Manager::Update()
{
	// 1. 각 콜라이더 업데이트 (Transform 적용)
	for (auto& pCollider : m_pColliders)
		pCollider->Update();

	// 2. 충돌 검사 (중복 검사 X, 자기 자신 검사 X)
	const size_t iNumColliders = m_pColliders.size();

	for (size_t i = 0; i < iNumColliders; ++i)
	{
		for (size_t j = i + 1; j < iNumColliders; ++j) // j = i + 1부터 시작
		{
			if (m_pColliders[i] == nullptr || m_pColliders[j] == nullptr)
				continue;

			if (m_pColliders[i]->Intersect(m_pColliders[j]))
			{
				m_pColliders[i]->Is_Hit(m_pColliders[j]); // 타겟 넘겨주는 것도 좋음
				m_pColliders[j]->Is_Hit(m_pColliders[i]);
			}
		}
	}
}


HRESULT CCollision_Manager::Render()
{
	for (auto& Collider : m_pColliders)
		Collider->Render();

	return S_OK;
}


void CCollision_Manager::Clear(_uint iCurrentLevelIndex)
{
	if (iCurrentLevelIndex >= m_iNumLevels)
		return;

	// 컨테이너 클리어
	for (auto& Collider : m_pColliders)
	{
		Safe_Release(Collider);
	}

	m_pColliders.clear();
}

CCollision_Manager* CCollision_Manager::Create(_uint iNumLevels)
{
	CCollision_Manager* pInstance = new CCollision_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CCollision_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollision_Manager::Free()
{
	__super::Free();

	// 저장한 Collision 해제
	
	Safe_Release(m_pGameInstance);
}
