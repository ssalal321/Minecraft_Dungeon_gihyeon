#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
	class CPlayer;

	class CPlayerHP final : public CUIObject
{
public:

	typedef struct PLAYERHP_DESC : public CUIObject::UIOBJECT_DESC
	{
		_float			fPlayTime;

		PLAYERHP_DESC(const _tchar* GameObjectTag, UI_STATE uiState,
			_float x, _float y, _float z, _float sizeX, _float sizeY,
			const wstring& textureTag, _float speedPerSec = 0.f, _float rotationPerSec = 0.f, _float playTime = 0.f)
			: UIOBJECT_DESC(GameObjectTag, uiState, x, y, z, sizeX, sizeY, textureTag,
							speedPerSec, rotationPerSec),  // 부모 생성자 호출
			  fPlayTime(playTime) { }

		//// 복사 생성자
		//PLAYERHP_DESC(const PLAYERHP_DESC& other)
		//	: UIOBJECT_DESC(other), fPlayTime(other.fPlayTime) { }

		~PLAYERHP_DESC() override = default;

	}PLAYERHP_DESC;

private:
	CPlayerHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerHP(const CPlayerHP& Prototype);
	~CPlayerHP() override = default;


public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;

	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;

	HRESULT		Render()							override;

private:
	CPlayer*			m_pPlayer = { nullptr };

	_float				m_fInverseMaxHP = {};
	_float				m_CutOffY		= {};

	PLAYERHP_DESC*		m_pDesc			= { nullptr };

	CTexture*			m_pTextureCom	= { nullptr };
	CShader*			m_pShaderCom	= { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom	= { nullptr };

private:
	HRESULT		Ready_PlayerHP_Components();

public:
	static	 CPlayerHP*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*			Clone(void* pArg)	override;
	void					Free()				override;
};

END