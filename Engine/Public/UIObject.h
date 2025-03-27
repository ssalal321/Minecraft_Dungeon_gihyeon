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
		_uint		iPrototypeLevelIndex, iLayerLevelIndex;
		_float		fX, fY, fZ, fSizeX, fSizeY;
		_wstring	strTextureComTag;

		tagUIObjectDesc(const _tchar* GameObjectTag, UI_STATE uiState, _uint PrototypeLevelIndex, _uint LayerLevelIndex,
			_float x, _float y, _float z, _float sizeX, _float sizeY, const wstring& textureTag, _float rotationPerSec = 0.f, _float speedPerSec = 0.f)
			: GAMEOBJECT_DESC(GameObjectTag, rotationPerSec, speedPerSec),
			eUIState(uiState), iPrototypeLevelIndex(PrototypeLevelIndex), iLayerLevelIndex(LayerLevelIndex),
			fX(x), fY(y), fZ(z), fSizeX(sizeX), fSizeY(sizeY), strTextureComTag(textureTag) {}

		tagUIObjectDesc(const tagUIObjectDesc& other)
			: GAMEOBJECT_DESC(other), eUIState(other.eUIState),
			  iPrototypeLevelIndex(other.iPrototypeLevelIndex), iLayerLevelIndex(other.iLayerLevelIndex),
			  fX(other.fX), fY(other.fY), fZ(other.fZ), fSizeX(other.fSizeX), fSizeY(other.fSizeY),
			  strTextureComTag(other.strTextureComTag) {}

		virtual ~tagUIObjectDesc() = default;

	}UIOBJECT_DESC;

protected:
	CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIObject(const CUIObject& Prototype);
	virtual ~CUIObject() = default;

public:
	virtual		HRESULT		Initialize_Prototype();
	virtual		HRESULT		Initialize(void* pArg);
	virtual		void		Priority_Update(_float fTimeDelta);
	virtual		void		Update(_float fTimeDelta);
	virtual		void		Late_Update(_float fTimeDelta);
	virtual		HRESULT		Render();

public:
	_bool	Is_Hovering(HWND hWnd);

	void	SetClickCallback(std::function<void()> callback) {
		m_ClickCallback = callback;
	}

	void	OnClick() {
		if (m_ClickCallback) {
			m_ClickCallback();  // 클릭 시 동작 실행
		}
	}

	void	Add_Child(CUIObject* child) {
		child->m_pParent = this;
		m_Children.push_back(child);
	}

	void	Set_Visible(bool visible) {
		m_bVisible = visible;
		for (auto& child : m_Children) {
			child->Set_Visible(visible);  // 부모가 꺼지면 자식도 꺼짐
		}
	}

protected:
	_float4x4		m_ViewMatrix = {};
	_float4x4		m_ProjMatrix = {};

	_float			m_fX{}, m_fY{}, m_fSizeX{}, m_fSizeY{};

	std::function<void()>	m_ClickCallback;  // 클릭 시 실행될 콜백 함수


	bool		m_bVisible	= true;
	CUIObject*	m_pParent	= { nullptr };
	std::vector<CUIObject*>		m_Children;  // 자식 리스트

protected:
	HRESULT		Bind_ShaderMatrices(CShader* pShader, const _char* pViewMatrixName, const _char* pProjMatrixName);


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END