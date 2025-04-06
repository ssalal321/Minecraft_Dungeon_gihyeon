#pragma once

#include "Base.h"

/* 화면에 그려야할 객체들을 모아놓는다. */
/* 그려야하는 순서대로 정리하여 모은다. */

BEGIN(Engine)

class CRenderer final : public CBase
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_NONBLEND, RENDER_BLEND, RENDER_UI, RENDER_END };

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderObject(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
	HRESULT Draw();

private:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };
	list<class CGameObject*>			m_RenderObjects[RENDER_END];

private:
	HRESULT Render_Priority();
	HRESULT Render_NonBlend();
	HRESULT Render_Blend();
	//HRESULT Render_UI();
	

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END