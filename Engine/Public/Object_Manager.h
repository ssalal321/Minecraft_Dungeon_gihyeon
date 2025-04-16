#pragma once

#include "Base.h"
/* 게임내에 사용하기위한 사본객체들을 레벨별로 레이어로 구분하여 보관한다. */

namespace Engine
{
	class CComponent;
	class CGameObject;
}

BEGIN(Engine)
	class CObject_Manager final : public CBase
{
private:
	CObject_Manager();
	~CObject_Manager() override = default;

public:
	HRESULT			Add_GameObject(_uint iPrototypeLevelIndex, const _wstring strPrototypeTag, 
							   _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg);
	CComponent*		Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex);
	CGameObject*	Find_GameObject(const _wstring strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag);

	HRESULT		Initialize(_uint iNumLevels);
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
	HRESULT				Add_To_Layer(CGameObject* pGameObject, _uint iLayerLevelIndex,
									 const _wstring& strLayerTag, const _wstring strPrototypeTag);

public:
	static CObject_Manager* Create(_uint iNumLevels);
	void	Free()		override;
};


END
