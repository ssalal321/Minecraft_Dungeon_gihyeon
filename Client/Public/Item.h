#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CItem abstract : public CPartObject
{
public:
	typedef struct tagItemDesc : public CPartObject::PARTOBJECT_DESC
	{
		const _uint*		pState = { nullptr };
		const _float4x4*	pSocketMatrix = { nullptr };

		_uint		uiHealPoint = {};
		_int		iDealPoint = {};
				
		_wstring	strIconTexPrototypeTag;
		_wstring	strIconGameObjectTag = {};

	}ITEM_DESC;

protected:
	CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem(const CItem& Prototype);
	~CItem() override = default;

public:
	const _uint&		Get_HealPoint() const { return m_uiHealPoint; }
	const _int&			Get_DealPoint() const { return m_iDealPoint; }
	const _wstring&		Get_IconTexPrototypeTag() { return m_strTexPrototypeTag; }
	const _wstring&		Get_IconGameObjectTag() { return m_strIconGameObjectTag; }
	const _wstring&		Get_ObjectPrototypeTag() { return m_strObjectPrototypeTag; }
	const _wstring&		Get_PartObjectTag() { return m_strPartObjectTag; }
	ITEM_TYPE			Get_ItemType() const { return m_eItemtype; }
	const _wstring&		Get_ColliderTag() { return m_strColliderTag; }

	const _wstring&		Get_ObjectTag() const { return m_strGameObjectTag; }

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

protected:	
	CShader*		m_pShaderCom = { nullptr };
	CModel*			m_pModelCom  = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };

	const  _float4x4*	m_pSocketMatrix = { nullptr };
	const  _uint*		m_pTargetState  = { nullptr };

	_int		m_uiHealPoint = { 0 };
	_int		m_iDealPoint = { 0 };
	_wstring	m_strTexPrototypeTag = {};
	_wstring	m_strIconGameObjectTag = {};
	_wstring	m_strObjectPrototypeTag = {};
	_wstring	m_strPartObjectTag = {};
	ITEM_TYPE	m_eItemtype = { ITEM_TYPE::NONE };

	_wstring	m_strColliderTag = {};
	static  _int	m_iItemID;

protected:
	virtual  HRESULT	Ready_Components() = 0;
	virtual  HRESULT	Bind_ShaderResources();

public:
	void	Free()		override;
};

END