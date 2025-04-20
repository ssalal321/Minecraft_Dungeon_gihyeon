#pragma once

#include "Base.h"

BEGIN(Engine)
	class CComponent;
	class CGameObject;

class CLayer final : public CBase
{
private:
	CLayer();
	~CLayer() override = default;

public:
	HRESULT			Add_GameObject(_wstring strLayerTag, CGameObject* pGameObject);
	CComponent*		Get_Component(const _wstring& strComponentTag, _uint iIndex);
	unordered_map<_wstring, CGameObject*>*	Get_LayerObjects() { return &m_GameObjects; }

	void			Priority_Update(_float fTimeDelta);
	void			Update(_float fTimeDelta);
	void			Late_Update(_float fTimeDelta);

	CGameObject*	Find_GameObject(_wstring strPrototypeTag);

private:
	unordered_map<_wstring, CGameObject*>	m_GameObjects;

public:
	static CLayer*	Create();
	virtual void	Free() override;
};

END