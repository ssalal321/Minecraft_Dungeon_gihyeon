#pragma once
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CHP_Bar final : public CUIObject
{
public:
	enum  UI_HPBAR_TYPE { PLAYERHP};

	typedef struct tagUIImageDesc : public CUIObject::UIOBJECT_DESC
	{
		UI_HPBAR_TYPE	eHPbarType;
		_uint			iPrototypeLevelIndex, iLayerLevelIndex;
		_float			fPlayTime;
		_wstring		strTextureComTag;

		tagUIImageDesc(const _tchar* GameObjectTag, UI_HPBAR_TYPE HPbarType, _uint PrototypeLevelIndex, _uint LayerLevelIndex,
			_float x, _float y, _float z, _float sizeX, _float sizeY,
			const wstring& textureTag, _float speedPerSec = 0.f, _float rotationPerSec = 0.f, _float playTime = 0.f)
			: UIOBJECT_DESC(GameObjectTag, x, y, z, sizeX, sizeY, speedPerSec, rotationPerSec),  // 부모 생성자 호출
			eHPbarType(HPbarType), iPrototypeLevelIndex(PrototypeLevelIndex), iLayerLevelIndex(LayerLevelIndex),
			strTextureComTag(textureTag), fPlayTime(playTime) {
		}

		// 복사 생성자
		tagUIImageDesc(const tagUIImageDesc& other)
			: UIOBJECT_DESC(other), eHPbarType(other.eHPbarType),
			iPrototypeLevelIndex(other.iPrototypeLevelIndex),
			iLayerLevelIndex(other.iLayerLevelIndex),
			fPlayTime(other.fPlayTime), strTextureComTag(other.strTextureComTag) {
		}

		~tagUIImageDesc() = default;
	}UI_HPBAR_DESC;

private:
	CHP_Bar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHP_Bar(const CHP_Bar& Prototype);
	~CHP_Bar() override = default;


public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;

	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Last_Update(_float fTimeDelta)		override;

	HRESULT		Render()							override;

private:
	_float				m_CutOffY		= {};

	UI_HPBAR_DESC*		m_pDesc			= { nullptr };

	CTexture*			m_pTextureCom	= { nullptr };
	CTexture*			m_pTextureCom_PlayerHP = { nullptr };
	CTexture*			m_pTextureCom_PlayerHP_Top = { nullptr };

	CTexture*			m_pTextureCom_MonsterHP = { nullptr };

	CShader*			m_pShaderCom	= { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom	= { nullptr };

private:
	HRESULT		Ready_PlayerHP_Components();
	HRESULT		Ready_PlayerHP_Top_Components();
	_bool		is_Hit_for_Test();
	_bool		Get_KeyDown();

public:
	static	 CHP_Bar*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*			Clone(void* pArg)	override;
	void					Free()				override;
};

END