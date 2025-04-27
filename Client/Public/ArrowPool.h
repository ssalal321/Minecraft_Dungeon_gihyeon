#pragma once

/* 클라이언트와 엔진 사이에 기능적인 소통을 위한 클래스. */
/* 엔진의 기능(함수)을 클라이언트에 보여주기 위한 클래스.  */
#include "Client_Defines.h"
#include "Base.h"
#include <queue>

namespace Engine
{
	class CGameInstance;
}

BEGIN(Client)
	class CArrow;

class CArrowPool final : public CBase
{
	DECLARE_SINGLETON(CArrowPool)
private:
	CArrowPool();
	~CArrowPool() override = default;

public:
	HRESULT		Initialize();
	void		Update(_float fTimeDelta);
	HRESULT		Draw();
	void		Clear(_uint iCurrentLevelIndex, _uint iNextLevelIndex);
	void		Release_ArrowPool();

public:
	// 화살 꺼내오기
	CArrow*		Get_Arrow();

	// 화살 반납하기
	void		Return_Arrow(CArrow* pArrow);

private:
	CGameInstance*	m_pGameInstance = { nullptr };

	queue<CArrow*>	m_ArrowPool;

public:
	void	Free()				override;
};

END
