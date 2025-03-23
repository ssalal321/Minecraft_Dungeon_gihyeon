#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
public:
	typedef struct tagUIObjectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float		fX, fY, fZ, fSizeX, fSizeY;

		tagUIObjectDesc(const _tchar* GameObjectTag,
			_float x, _float y, _float z, _float sizeX, _float sizeY, _float rotationPerSec = 0.f, _float speedPerSec = 0.f)
			: GAMEOBJECT_DESC(GameObjectTag, rotationPerSec, speedPerSec),
			fX(x), fY(y), fZ(z), fSizeX(sizeX), fSizeY(sizeY) {}

		tagUIObjectDesc(const tagUIObjectDesc& other)
			: GAMEOBJECT_DESC(other),
			  fX(other.fX), fY(other.fY), fZ(other.fZ), fSizeX(other.fSizeX), fSizeY(other.fSizeY) {}
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
	virtual		void		Last_Update(_float fTimeDelta);
	virtual		HRESULT		Render();

public:
	_bool	isHit(HWND hWnd);

protected:
	_float4x4		m_ViewMatrix = {};
	_float4x4		m_ProjMatrix = {};

	_float			m_fX{}, m_fY{}, m_fSizeX{}, m_fSizeY{};

protected:
	HRESULT Bind_ShaderMatrices(CShader* pShader, const _char* pViewMatrixName, const _char* pProjMatrixName);


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END