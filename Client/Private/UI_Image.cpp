#include "UI_Image.h"
#include "GameInstance.h"

CUI_Image::CUI_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CUI_Image::CUI_Image(const CUI_Image& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CUI_Image::Initialize_Prototype()
{
	/* 외부 데이터베이스를 통해서 값을 채운다. */

	return S_OK;
}

HRESULT CUI_Image::Initialize(void* pArg)
{
	/* 원형의 데이터를 복제하여 사본을 만들고. */
	/* 추가적으로 필요한 데이터를 Arg로 받아와 실 사용하기위한 객체의 정보를 생성해준다. */
	if (nullptr != pArg)
	{
		m_pDesc = static_cast<UIIMAGE_DESC*>(pArg);
	}
	else
		return E_FAIL;

	if (FAILED(__super::Initialize(m_pDesc)))
		return E_FAIL;

	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	m_pTransformCom->SetUp_Scale(m_pDesc->fSizeX, m_pDesc->fSizeY);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_pDesc->fX - ViewportDesc.Width * 0.5f, -m_pDesc->fY + ViewportDesc.Height * 0.5f, 0.f, 1.f));
	
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUI_Image::Priority_Update(_float fTimeDelta)
{

}

void CUI_Image::Update(_float fTimeDelta)
{
	/*_bool		isClicked = {};

	if (GetKeyState(VK_LBUTTON) & 0x8000)
		isClicked = isHit(g_hWnd);*/

}

void CUI_Image::Last_Update(_float fTimeDelta)
{


	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Image::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(Bind_ShaderMatrices(m_pShaderCom, "g_ViewMatrix", "g_ProjMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;


	m_pVIBufferCom->Input_Assembler();
	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

_bool CUI_Image::isHit(HWND hWnd)
{
	POINT		ptMouse{};

	GetCursorPos(&ptMouse);

	ScreenToClient(hWnd, &ptMouse);

	RECT		rcUI = { static_cast<_long>(m_pDesc->fX - m_pDesc->fSizeX * 0.5f),
		static_cast<_long>(m_pDesc->fY - m_pDesc->fSizeY * 0.5f),
		static_cast<_long>(m_pDesc->fX + m_pDesc->fSizeX * 0.5f),
		static_cast<_long>(m_pDesc->fY + m_pDesc->fSizeY * 0.5f)
	};

	return PtInRect(&rcUI, ptMouse);
}


HRESULT CUI_Image::Ready_Components()
{
	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_TITLE, m_pDesc->strTextureComTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(& m_pTextureCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_TITLE, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_TITLE, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Image::Bind_ShaderMatrices(CShader* pShader, const _char* pViewMatrixName, const _char* pProjMatrixName)
{
	if (FAILED(pShader->Bind_Matrix(pViewMatrixName, &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(pShader->Bind_Matrix(pProjMatrixName, &m_ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

CUI_Image* CUI_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Image* pGameInstance = new CUI_Image(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUI_Image");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject* CUI_Image::Clone(void* pArg)
{
	CUI_Image* pGameInstance = new CUI_Image(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CUI_Image");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CUI_Image::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
