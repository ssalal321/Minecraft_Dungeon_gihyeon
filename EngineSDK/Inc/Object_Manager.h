#pragma once

#include "Base.h"
/* 게임내에 사용하기위한 사본객체들을 레벨별로 레이어로 구분하여 보관한다. */

BEGIN(Engine)

class CObject_Manager final : public CBase
{
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	HRESULT		Initialize(_uint iNumLevels);
	HRESULT		Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, 
							   _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg);
	void		Priority_Update(_float fTimeDelta);
	void		Update(_float fTimeDelta);
	void		Late_Update(_float fTimeDelta);
	void		Clear(_uint iLevelIndex);

private:
	class CGameInstance*	m_pGameInstance = { nullptr };
	_uint					m_iNumLevels = {};

	map<const _wstring, class CLayer*>*		m_pLayers = { nullptr };

private:
	class	CLayer*		Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag);
	HRESULT				Add_To_Layer(class CGameObject* pGameObject, _uint iLayerLevelIndex, const _wstring& strLayerTag);

public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END