#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_Image final : public CUIObject
{
public:
	typedef struct tagUIImageDesc : public CUIObject::UIOBJECT_DESC
	{
		_uint	iPrototypeLevelIndex, iLayerLevelIndex;

		tagUIImageDesc(const _tchar* GameObjectTag, UI_STATE uiState,
			_float x, _float y, _float z, _float sizeX, _float sizeY, const wstring& textureTag,
			_uint PrototypeLevelIndex, _uint LayerLevelIndex,
			_float rotationPerSec = 0.f, _float speedPerSec = 0.f, _float playTime = 0.f)
			: UIOBJECT_DESC(GameObjectTag, uiState,
				x, y, z, sizeX, sizeY, textureTag, rotationPerSec, speedPerSec),  // 부모 생성자 호출
			 iPrototypeLevelIndex(PrototypeLevelIndex), iLayerLevelIndex(LayerLevelIndex) {
		}

		//// 복사 생성자
		//tagUIImageDesc(const tagUIImageDesc& other)
		//	: UIOBJECT_DESC(other),
		//	  iPrototypeLevelIndex(other.iPrototypeLevelIndex), iLayerLevelIndex(other.iLayerLevelIndex) {
		//}

		~tagUIImageDesc() override = default;

	}UIIMAGE_DESC;

private:
	CUI_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Image(const CUI_Image& Prototype);
	~CUI_Image() override = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;

	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;

	HRESULT		Render()							override;

protected:
	/*_bool	Is_Hovering() override;
	_bool	Is_KeyDown() override;
	_bool	Is_KeyUp() override;
	_bool	Is_KeyPressing() override;*/

private:
	UIIMAGE_DESC*	m_pDesc = { nullptr };

	//_float				m_fPlayTime{};

	CTexture*			m_pTextureCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };

private:
	HRESULT		Ready_Components();
	/*_bool	Is_Hovering() override { return true; };
	_bool	Is_KeyDown() override { return true; };
	_bool	Is_KeyUp() override { return true; };
	_bool	Is_KeyPressing() override { return true; };*/

public:
	static	 CUI_Image* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*		Clone(void* pArg)	override;
	void	Free()							override;
};

END