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

		_wstring			strTexPrototypeTag;
		ITEMTYPE			eItemtype;

	}ITEM_DESC;

protected:
	CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem(const CItem& Prototype);
	~CItem() override = default;

public:
	const _wstring&  Get_TexPrototypeTag() { return m_strTexPrototypeTag; }
	ITEMTYPE	Get_ItemType() const { return m_eItemtype; }

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

protected:
	const  _float4x4*	m_pSocketMatrix = { nullptr };
	const  _uint*		m_pTargetState  = { nullptr };

	_wstring	m_strTexPrototypeTag = {};
	ITEMTYPE	m_eItemtype = { ITEMTYPE::NONE };

protected:
	virtual  HRESULT	Ready_Components() = 0;
	virtual  HRESULT	Bind_ShaderResources();

public:
	void	Free()		override;
};

END