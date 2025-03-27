#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CContainerObject abstract : public CGameObject
{
protected:
	CContainerObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CContainerObject(const CContainerObject& Prototype);
	virtual ~CContainerObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	map<const _wstring, class CPartObject*>				m_PartObjects;

protected:
	CPartObject* Find_PartObject(const _wstring& strPartObjectTag);
	HRESULT Add_PartObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strPartObjectTag, void* pArg = nullptr);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END