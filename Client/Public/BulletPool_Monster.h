#pragma once

/* 클라이언트와 엔진 사이에 기능적인 소통을 위한 클래스. */
/* 엔진의 기능(함수)을 클라이언트에 보여주기 위한 클래스.  */
#include "Client_Defines.h"
#include "Base.h"
#include <queue>


BEGIN(Engine)
	class CGameInstance;
END

BEGIN(Client)
class CSlime_Cauldron_Bullet;

class CBulletPool_Monster final : public CBase
{
private:
	CBulletPool_Monster();
	~CBulletPool_Monster() override = default;

public:
	HRESULT		Initialize();
	/*void		Update(_float fTimeDelta);
	HRESULT		Draw();*/
	void		Clear(_uint iCurrentLevelIndex, _uint iNextLevelIndex);

public:
	// 화살 꺼내오기
	CSlime_Cauldron_Bullet*		Get_Bullet(_uint uiDealPoint);

	// 화살 반납하기
	void		Return_Bullet(CSlime_Cauldron_Bullet* pBullet);

private:
	CGameInstance*	m_pGameInstance = { nullptr };

	queue<CSlime_Cauldron_Bullet*>	m_MonsterBulletPool;

public:
	static  CBulletPool_Monster*  Create();
	void	Free()				override;
};

END
