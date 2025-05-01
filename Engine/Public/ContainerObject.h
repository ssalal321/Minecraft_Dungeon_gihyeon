#pragma once

#include "GameObject.h"

BEGIN(Engine)
	class CPartObject;

	class ENGINE_DLL CContainerObject abstract : public CGameObject
{
protected:
	CContainerObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CContainerObject(const CContainerObject& Prototype);
	~CContainerObject() override = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

public:
	CPartObject*	Find_PartObject(const _wstring& strPartObjectTag);
	CComponent*		Find_Part_Component(const _wstring& strPartObjectTag, const _wstring& strComponentTag);

protected:
	map<const _wstring, class CPartObject*>		m_PartObjects;
	_bool*		m_pAttacking = { nullptr };

protected:
	HRESULT			Add_PartObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strPartObjectTag, void* pArg = nullptr);

public:
	CGameObject* Clone(void* pArg)	override = 0;
	void	Free()					override;
};

END