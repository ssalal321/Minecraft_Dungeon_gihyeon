#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
public:
	enum  UI_STATE { CLICKABLE, UNCLICKABLE };

	typedef struct tagUIObjectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		UI_STATE	eUIState;
		_float		fX, fY, fZ, fSizeX, fSizeY;
		_wstring	strTextureComTag;

		tagUIObjectDesc(const _tchar* GameObjectTag, UI_STATE uiState,
			_float x, _float y, _float z, _float sizeX, _float sizeY, const wstring& textureTag, _float rotationPerSec = 0.f, _float speedPerSec = 0.f)
			: GAMEOBJECT_DESC(GameObjectTag, rotationPerSec, speedPerSec),
			eUIState(uiState), fX(x), fY(y), fZ(z), fSizeX(sizeX), fSizeY(sizeY), strTextureComTag(textureTag) {
}

tagUIObjectDesc(const tagUIObjectDesc& other)
	: GAMEOBJECT_DESC(other), eUIState(other.eUIState),
	  fX(other.fX), fY(other.fY), fZ(other.fZ), fSizeX(other.fSizeX), fSizeY(other.fSizeY),
	  strTextureComTag(other.strTextureComTag) {
}

~tagUIObjectDesc() override = default;

}UIOBJECT_DESC;

protected:
	CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIObject(const CUIObject& Prototype);
	~CUIObject() override = default;

public:
	HRESULT		Initialize_Prototype()					override;
	HRESULT		Initialize(void* pArg)					override;
	void		Priority_Update(_float fTimeDelta)		override;
	void		Update(_float fTimeDelta)				override;
	void		Late_Update(_float fTimeDelta)			override;
	HRESULT		Render()								override;

public:
	void	Set_Parent(CUIObject* pParent)
	{
		m_pParent = pParent;
	}

	// UI가 보이는지 여부를 반환하는 함수
	_bool	Is_Visible() const;

	// UI의 표시 상태 설정
	void	Set_Visible(_bool bVisible)
	{
		m_bVisible = bVisible;
	}

protected:
	_float4x4		m_ViewMatrix = {};
	_float4x4		m_ProjMatrix = {};

	_float			m_fX{}, m_fY{}, m_fSizeX{}, m_fSizeY{};

	bool			m_bVisible = false;

	CUIObject*		m_pParent = { nullptr };

protected:
	HRESULT		Bind_ShaderMatrices(CShader* pShader, const _char* pViewMatrixName, const _char* pProjMatrixName);

protected:
	virtual _bool	Is_Hovering();
	virtual _bool	Is_KeyDown();
	virtual _bool	Is_KeyUp();
	virtual _bool	Is_KeyPressing();

public:
	CGameObject* Clone(void* pArg)	override = 0;
	void	Free()					override;
};

END