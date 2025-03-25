#pragma once

#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Engine)

class ENGINE_DLL CUI_Image : public CUIObject
{
public:
	typedef struct tagUIImageDesc : public CUIObject::UIOBJECT_DESC
	{
		_float		fPlayTime;

		tagUIImageDesc(const _tchar* GameObjectTag, UI_STATE uiState, 
			_uint prototypeLevelIndex, _uint layerLevelIndex, 
			_float x, _float y, _float z, _float sizeX, _float sizeY,
			const wstring& textureTag, _float rotationPerSec = 0.f, _float speedPerSec = 0.f, _float playTime = 0.f)
			: UIOBJECT_DESC(GameObjectTag, uiState, prototypeLevelIndex, layerLevelIndex,
							x, y, z, sizeX, sizeY, textureTag, rotationPerSec, speedPerSec),  // 부모 생성자 호출
			  fPlayTime(playTime) {
		}

		// 복사 생성자
		tagUIImageDesc(const tagUIImageDesc& other)
			: UIOBJECT_DESC(other),
			fPlayTime(other.fPlayTime) { }

		~tagUIImageDesc() = default;
	}UIIMAGE_DESC;

private:
	CUI_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Image(const CUI_Image& Prototype);
	virtual ~CUI_Image() = default;

public:
	virtual   HRESULT	Initialize_Prototype()				override;
	virtual   HRESULT	Initialize(void* pArg)				override;

	virtual   void		Priority_Update(_float fTimeDelta)	override;
	virtual   void		Update(_float fTimeDelta)			override;
	virtual   void		Late_Update(_float fTimeDelta)		override;

	virtual	  HRESULT	Render()							override;

public:
	_bool		is_Hovering();

protected:
	_bool		Is_KeyDown();
	_bool		Is_KeyUp();
	_bool		Is_KeyPressing();


private:
	UIIMAGE_DESC*	m_pDesc = { nullptr };

	//_float				m_fPlayTime{};

	CTexture*			m_pTextureCom	= { nullptr };
	CShader*			m_pShaderCom	= { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom	= { nullptr };

private:
	HRESULT		Ready_Components();
	//HRESULT		Bind_ShaderMatrices(CShader* pShader, const _char* pViewMatrixName, const _char* pProjMatrixName);

public:
	static	 CUI_Image*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*			Clone(void* pArg)	override;
	void					Free()				override;
};

END