#pragma once
#include "Client_Defines.h"
#include "Monster.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CCauldronBoss;

class CCauldronBossHP final : public CUIObject
{
public:

	typedef struct CAULDRONBOSS_HP_DESC : public CUIObject::UIOBJECT_DESC
	{
		_float				fPlayTime;
		CCauldronBoss*		pBoss = { nullptr };

		CAULDRONBOSS_HP_DESC(const _tchar* GameObjectTag, UI_STATE uiState,
			_float x, _float y, _float z, _float sizeX, _float sizeY,
			const wstring& textureTag, CCauldronBoss* pMonster, _bool alphaBlend = true, _float speedPerSec = 0.f, _float rotationPerSec = 0.f, _float playTime = 0.f)
			: UIOBJECT_DESC(GameObjectTag, uiState, x, y, z, sizeX, sizeY, textureTag, alphaBlend, rotationPerSec, speedPerSec),  // 부모 생성자 호출
			  fPlayTime(playTime), pBoss(pMonster) { }

		//// 복사 생성자
		//CAULDRONBOSS_HP_DESC(const CAULDRONBOSS_HP_DESC& other)
		//	: UIOBJECT_DESC(other), fPlayTime(other.fPlayTime) { }

		~CAULDRONBOSS_HP_DESC() override = default;

	}CAULDRONBOSS_HP_DESC;

private:
	CCauldronBossHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCauldronBossHP(const CCauldronBossHP& Prototype);
	~CCauldronBossHP() override = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;

	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;

	HRESULT		Render()							override;

private:
	//CCauldronBoss*		m_pCauldronBoss = { nullptr };

	CMonster::MONSTER_DESC*		m_pMonsterDesc = { nullptr };
	CCauldronBoss*				m_pCauldronBoss = { nullptr };
	CPartObject*			m_pBody_CauldronBoss = { nullptr };

	_float				m_fInverseMaxHP = {};
	_float				m_CutOffX		= {};

	CAULDRONBOSS_HP_DESC*	m_pDesc			= { nullptr };

	CTexture*			m_pTextureCom	= { nullptr };
	CShader*			m_pShaderCom	= { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom	= { nullptr };

	_float2				m_vScreenPos = {};

private:
	HRESULT		Ready_CauldronBossHP_Components();

public:
	static	 CCauldronBossHP*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*			Clone(void* pArg)	override;
	void					Free()				override;
};

END