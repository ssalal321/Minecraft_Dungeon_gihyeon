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
	_bool		Get_Stationary() const { return m_bStationary; }

	void		Set_Stationary(_bool bStationary) { m_bStationary = bStationary; }

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
	HRESULT			Add_PartObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strPartObjectTag, void* pArg = nullptr);
	HRESULT			Add_PartObject(CPartObject* pPartObject, const _wstring& strPartObjectTag);
	HRESULT			Delete_PartObject(const _wstring& strPartObjectTag, _bool bReleaseMemory);
	void			Resolve_Penetration_And_Slide(CCollider* pOther, _float fForce);
	

	//void			Apply_PushBack(const _float4& vFromPosition, _float fForce, CNavigation* pNavigation);

protected:
	map<const _wstring, class CPartObject*>		m_PartObjects;
	_bool*		m_pCollisionActivating = { nullptr };

	_bool		m_bStationary = { false };

public:
	CGameObject* Clone(void* pArg)	override = 0;
	void	Free()					override;
};

END