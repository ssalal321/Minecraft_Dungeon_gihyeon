#include "UIObject.h"
#include "Shader.h"

CUIObject::CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject { pDevice, pContext }
{

}

CUIObject::CUIObject(const CUIObject& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CUIObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIObject::Initialize(void* pArg)
{
	UIOBJECT_DESC* pDesc = static_cast<UIOBJECT_DESC*>(pArg);
	if (nullptr == pDesc)
		return E_FAIL;	

	m_fX = pDesc->fX;
	m_fY = pDesc->fY;
	m_fSizeX = pDesc->fSizeX;
	m_fSizeY = pDesc->fSizeY;

	if (FAILED(__super::Initialize(pDesc)))	
		return E_FAIL;

	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	m_pTransformCom->SetUp_Scale(m_fSizeX, m_fSizeY);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, 
		XMVectorSet(m_fX - ViewportDesc.Width * 0.5f, -m_fY + ViewportDesc.Height * 0.5f, pDesc->fZ, 1.f));

	return S_OK;
}

void CUIObject::Priority_Update(_float fTimeDelta)
{
}

void CUIObject::Update(_float fTimeDelta)
{
}

void CUIObject::Last_Update(_float fTimeDelta)
{
}

HRESULT CUIObject::Render()
{
	return S_OK;
}

_bool CUIObject::isHit(HWND hWnd)
{
	POINT		ptMouse{};

	GetCursorPos(&ptMouse);

	ScreenToClient(hWnd, &ptMouse);

	RECT		rcUI = { static_cast<_long>(m_fX - m_fSizeX * 0.5f), 
		static_cast<_long>(m_fY - m_fSizeY * 0.5f),
		static_cast<_long>(m_fX + m_fSizeX * 0.5f), 
		static_cast<_long>(m_fY + m_fSizeY * 0.5f) 
	};

	return PtInRect(&rcUI, ptMouse);	
}


HRESULT CUIObject::Bind_ShaderMatrices(CShader* pShader, const _char* pViewMatrixName, const _char* pProjMatrixName)
{
	if (FAILED(pShader->Bind_Matrix(pViewMatrixName, &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(pShader->Bind_Matrix(pProjMatrixName, &m_ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

void CUIObject::Free()
{
	__super::Free();

}
