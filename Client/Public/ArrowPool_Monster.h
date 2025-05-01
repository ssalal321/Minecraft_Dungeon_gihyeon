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
class CMonster_Arrow;

class CArrowPool_Monster final : public CBase
{
private:
	CArrowPool_Monster();
	~CArrowPool_Monster() override = default;

public:
	HRESULT		Initialize();
	/*void		Update(_float fTimeDelta);
	HRESULT		Draw();*/
	void		Clear(_uint iCurrentLevelIndex, _uint iNextLevelIndex);

public:
	// 화살 꺼내오기
	CMonster_Arrow*		Get_Arrow(_uint uiDealPoint);

	// 화살 반납하기
	void		Return_Arrow(CMonster_Arrow* pArrow);

private:
	CGameInstance*	m_pGameInstance = { nullptr };

	queue<CMonster_Arrow*>	m_MonsterArrowPool;

public:
	static  CArrowPool_Monster*  Create();
	void	Free()				override;
};

END
