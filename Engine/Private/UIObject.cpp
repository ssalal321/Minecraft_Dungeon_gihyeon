#include "UIObject.h"
#include "Shader.h"
#include "GameInstance.h"

CUIObject::CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{

}

CUIObject::CUIObject(const CUIObject& Prototype)
	: CGameObject(Prototype)
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

	m_eUIState		= pDesc->eUIState;
	m_fX			= pDesc->fX;
	m_fY			= pDesc->fY;
	m_fZ			= pDesc->fZ;
	m_fSizeX		= pDesc->fSizeX;
	m_fSizeY		= pDesc->fSizeY;
	m_strTexPrototypeTag = pDesc->strTexPrototypeTag;
	m_bAlphaBlend	= pDesc->bAlphaBlend;

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

void CUIObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CUIObject::Render()
{
	return S_OK;
}

_bool CUIObject::Is_Visible() const
{
	if (nullptr == m_pParent)
		return m_bVisible;

	// 부모가 있을 때 부모가 보이지 않으면 자식도 보이지 않음
	if (m_pParent && !m_pParent->Is_Visible())
		return false;

	return true;  // 부모가 보이면 자식은 자신의 m_bVisible 값에 따라 표시
}

HRESULT CUIObject::Bind_ShaderMatrices(CShader* pShader, const _char* pViewMatrixName, const _char* pProjMatrixName)
{
	if (FAILED(pShader->Bind_Matrix(pViewMatrixName, &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(pShader->Bind_Matrix(pProjMatrixName, &m_ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

_bool CUIObject::Is_Hovering()
{
	_float3		ptMouse = m_pGameInstance->Get_MousePos();
	POINT		MousePosition{ static_cast<_long>(ptMouse.x), static_cast<_long>(ptMouse.y) };

	RECT		rcUI = { static_cast<_long>(m_fX - m_fSizeX * 0.5f),
						 static_cast<_long>(m_fY - m_fSizeY * 0.5f),
						 static_cast<_long>(m_fX + m_fSizeX * 0.5f),
						 static_cast<_long>(m_fY + m_fSizeY * 0.5f)
	};

	return PtInRect(&rcUI, MousePosition);
}

_bool CUIObject::Is_KeyDown()
{
	return Is_Hovering() && m_pGameInstance->Key_Down(VK_LBUTTON);
}

_bool CUIObject::Is_KeyUp()
{
	return Is_Hovering() && m_pGameInstance->Key_Up(VK_LBUTTON);
}

_bool CUIObject::Is_KeyPressing()
{
	return Is_Hovering() && m_pGameInstance->Key_Pressing(VK_LBUTTON);
}

void CUIObject::Free()
{
	__super::Free();

}
