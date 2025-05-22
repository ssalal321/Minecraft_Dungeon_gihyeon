#pragma once
#include "Client_Defines.h"
#include "Player.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CPlayer;

class CHealthPotion final : public CUIObject
{
public:

	typedef struct HEALTHPOTION_ICON_DESC : public CUIObject::UIOBJECT_DESC
	{
		_float			fPlayTime;

		HEALTHPOTION_ICON_DESC(const _tchar* GameObjectTag, UI_STATE uiState,
			_float x, _float y, _float z, _float sizeX, _float sizeY,
			const wstring& textureTag, _bool alphaBlend = true, _float speedPerSec = 0.f, _float rotationPerSec = 0.f, _float playTime = 0.f)
			: UIOBJECT_DESC(GameObjectTag, uiState, x, y, z, sizeX, sizeY, textureTag, alphaBlend, rotationPerSec, speedPerSec),
			  fPlayTime(playTime) { }

		//// 복사 생성자
		//HEALTHPOTION_ICON_DESC(const HEALTHPOTION_ICON_DESC& other)
		//	: UIOBJECT_DESC(other), fPlayTime(other.fPlayTime) { }

		~HEALTHPOTION_ICON_DESC() override = default;

	}ROLL_ICON_DESC;

private:
	CHealthPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHealthPotion(const CHealthPotion& Prototype);
	~CHealthPotion() override = default;


public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;

	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;

	HRESULT		Render()							override;

private:
	CPlayer*			m_pPlayer = { nullptr };
	CPlayer::PLAYER_DESC*	m_pPlayerInfo = { nullptr };

	_float				m_fCooldownRatio = {};

	ROLL_ICON_DESC*		m_pDesc			= { nullptr };

	CTexture*			m_pTextureCom	= { nullptr };
	CShader*			m_pShaderCom	= { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom	= { nullptr };

private:
	HRESULT		Ready_Components();

public:
	static	 CHealthPotion*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*			Clone(void* pArg)	override;
	void					Free()				override;
};

END