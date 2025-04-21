#pragma once

#include "Base.h"

/* 현재 할당되어 보여지는 레벨의 주소를 들고 있는다. (참조x, 저장o)*/
/* + 기존 할당되어있던 레벨을 파괴한다. */
/* + 기존 레벨용 자원을 파괴한다. */

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
private:
	CLevel_Manager();
	~CLevel_Manager()	override;

public:
	_uint		Get_CurrentLevelIndex() const { return m_iLevelIndex; }
	_uint		Get_PrototypeLevelIndex() const { return m_iNextLevelIndex; }

	void		Set_PrototypeLevelIndex(_uint iNextLevelIndex) { m_iNextLevelIndex = iNextLevelIndex; }

public:
	HRESULT		Initialize();
	HRESULT		Open_Level(_uint iNextLevelIndex, class CLevel* pNewLevel);
	void		Update(_float fTimeDelta);
	HRESULT		Render();

private:
	_uint					m_iLevelIndex = {};
	_uint					m_iNextLevelIndex = {};
	class CLevel*			m_pCurrentLevel = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };

public:
	static CLevel_Manager* Create();
	void	Free()	override;
};

END