#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CTitleBase final : public CUIObject
{
public:
	typedef struct tagTitleBase : public CUIObject::UIOBJECT_DESC
	{
		_float		fPlayTime;

	}TITLEBASE_DESC;

private:
	CTitleBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTitleBase(const CTitleBase& Prototype);
	virtual ~CTitleBase() = default;

public:
	virtual		HRESULT		Initialize_Prototype() override;
	virtual		HRESULT		Initialize(void* pArg) override;

	virtual		void		Priority_Update(_float fTimeDelta) override;
	virtual		void		Update(_float fTimeDelta) override;
	virtual		void		Late_Update(_float fTimeDelta) override;

	virtual		HRESULT		Render() override;

private:
	_float				m_fPlayTime		= {  };

	CTexture*			m_pTextureCom	= { nullptr };
	CShader*			m_pShaderCom	= { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom	= { nullptr };

private:
	HRESULT		Ready_Components();

public:
	static	  CTitleBase*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual	  CGameObject*	Clone(void* pArg)	override;
	virtual	  void			Free()				override;
};

END