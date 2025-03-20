#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Engine)

class ENGINE_DLL CUI_Image final : public CGameObject
{
public:
	enum  UI_STATE { CLICKABLE, UNCLICKABLE };

	typedef struct tagUIImageDesc : public CGameObject::GAMEOBJECT_DESC
	{
		UI_STATE	eUIState;
		_uint		iPrototypeLevelIndex, iLayerLevelIndex;
		_float		fX, fY, fSizeX, fSizeY, fPlayTime;
		_wstring	strTextureComTag;

		tagUIImageDesc(const _tchar* gameObjectTag, UI_STATE uiState, _uint PrototypeLevelIndex, _uint LayerLevelIndex,
			_float x, _float y, _float sizeX, _float sizeY,
			const wstring& textureTag, _float rotationPerSec = 0.f, _float speedPerSec = 0.f, _float playTime = 0.f)
			: GAMEOBJECT_DESC(gameObjectTag, rotationPerSec, speedPerSec),  // 부모 생성자 호출
			eUIState(uiState), iPrototypeLevelIndex(PrototypeLevelIndex), iLayerLevelIndex(LayerLevelIndex),
			fX(x), fY(y), fSizeX(sizeX), fSizeY(sizeY),
			strTextureComTag(textureTag), fPlayTime(playTime) {
		}

		// 복사 생성자
		tagUIImageDesc(const tagUIImageDesc& other)
			: GAMEOBJECT_DESC(other.pGameObjectTag,
			other.fSpeedPerSec, other.fRotationPerSec),
			eUIState(other.eUIState), iPrototypeLevelIndex(other.iPrototypeLevelIndex),
			iLayerLevelIndex(other.iLayerLevelIndex),
			fX(other.fX), fY(other.fY),
			fSizeX(other.fSizeX), fSizeY(other.fSizeY),
			strTextureComTag(other.strTextureComTag), fPlayTime(other.fPlayTime) {
		}

		~tagUIImageDesc() = default;
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
	_bool		is_Hit();

protected:
	_bool	Get_KeyDown();
	_bool	Get_KeyUp();
	_bool	Get_KeyPressing();


private:
	UIIMAGE_DESC*	m_pDesc = { nullptr };

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