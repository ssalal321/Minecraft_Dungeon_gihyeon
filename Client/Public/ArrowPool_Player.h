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
class CPlayer_Arrow;

class CArrowPool_Player final : public CBase
{
private:
	CArrowPool_Player();
	~CArrowPool_Player() override = default;

public:
	HRESULT		Initialize();
	/*void		Update(_float fTimeDelta);
	HRESULT		Draw();*/
	void		Clear(_uint iCurrentLevelIndex, _uint iNextLevelIndex);

public:
	// 화살 꺼내오기
	CPlayer_Arrow*		Get_Arrow(_uint uiAttackPoint);

	// 화살 반납하기
	void		Return_Arrow(CPlayer_Arrow* pArrow);

private:
	CGameInstance*	m_pGameInstance = { nullptr };

	queue<CPlayer_Arrow*>	m_PlayerArrowPool;

public:
	static  CArrowPool_Player*  Create();
	void	Free()				override;
};

END
