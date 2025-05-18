#include "BulletPool_Monster.h"
#include "Slime_Cauldron_Bullet.h"
#include "GameInstance.h"

CBulletPool_Monster::CBulletPool_Monster() : m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CBulletPool_Monster::Initialize()
{
	for (_int i = 0; i < 3; ++i)
	{
		CGameObject* pBulletObject = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Slime_Cauldron_Bullet"),
													LEVEL_STATIC, TEXT("Layer_Bullet"));
		if (nullptr == pBulletObject)
			return E_FAIL;

		CSlime_Cauldron_Bullet*	 pBullet = dynamic_cast<CSlime_Cauldron_Bullet*>(pBulletObject);
		pBullet->Set_Pool(this);
		m_MonsterBulletPool.push(pBullet);
	}

	return S_OK;
}

void CBulletPool_Monster::Clear(_uint iCurrentLevelIndex, _uint iNextLevelIndex)
{
	while (!m_MonsterBulletPool.empty())
	{
		CSlime_Cauldron_Bullet* pBullet = m_MonsterBulletPool.front();
		m_MonsterBulletPool.pop();
		Safe_Release(pBullet);
	}
}

CSlime_Cauldron_Bullet* CBulletPool_Monster::Get_Bullet(_uint uiDealPoint)
{
	if (m_MonsterBulletPool.empty())
	{
		// 풀에 없으면 새로 생성
		CGameObject* pBulletObject = m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_Slime_Cauldron_Bullet"),
			LEVEL_STATIC, TEXT("Layer_Bullet"));
		if (nullptr == pBulletObject)	
			return nullptr;

		CSlime_Cauldron_Bullet* pNewBullet = dynamic_cast<CSlime_Cauldron_Bullet*>(pBulletObject);
		pNewBullet->Set_Pool(this);
		m_MonsterBulletPool.push(pNewBullet);

		pNewBullet = m_MonsterBulletPool.front();
		m_MonsterBulletPool.pop();  // 맨 앞에 있는 걸 꺼내고 제거
		pNewBullet->Set_DealPoint(uiDealPoint);

		return pNewBullet;
	}
	else
	{
		// 있으면 재사용
		CSlime_Cauldron_Bullet*  pBullet = m_MonsterBulletPool.front();
		m_MonsterBulletPool.pop();
		pBullet->Set_DealPoint(uiDealPoint);

		return pBullet;
	}
}

void CBulletPool_Monster::Return_Bullet(CSlime_Cauldron_Bullet* pBullet)
{
	if (pBullet)
	{
		m_MonsterBulletPool.push(pBullet);
	}
}

CBulletPool_Monster* CBulletPool_Monster::Create()
{
	CBulletPool_Monster* pGameInstance = new CBulletPool_Monster();

	if (FAILED(pGameInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CBulletPool_Monster");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CBulletPool_Monster::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
