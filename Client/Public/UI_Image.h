#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_Image final : public CGameObject
{
public:
	typedef struct tagUIObjectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float		fX, fY, fSizeX, fSizeY, fPlayTime;
		_wstring	strTextureComTag;

	}UIIMAGE_DESC;

private:
	CUI_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Image(const CUI_Image& Prototype);
	virtual ~CUI_Image() = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;

	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Last_Update(_float fTimeDelta)		override;

	HRESULT		Render()							override;

public:
	_bool		isHit(HWND hWnd);

private:
	UIIMAGE_DESC*	m_pDesc;

	_float4x4		m_ViewMatrix = {};
	_float4x4		m_ProjMatrix = {};

	_float			m_fX{}, m_fY{}, m_fSizeX{}, m_fSizeY{} ,m_fPlayTime{};

	CTexture*			m_pTextureCom	= { nullptr };
	CShader*			m_pShaderCom	= { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom	= { nullptr };

private:
	HRESULT		Ready_Components();
	HRESULT		Bind_ShaderMatrices(CShader* pShader, const _char* pViewMatrixName, const _char* pProjMatrixName);

public:
	static	 CUI_Image*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*			Clone(void* pArg)	override;
	void					Free()				override;
};

END