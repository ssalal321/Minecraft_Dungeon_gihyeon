#include "UI_Image.h"
#include "GameInstance.h"


CUI_Image::CUI_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject(pDevice, pContext)
{

}

CUI_Image::CUI_Image(const CUI_Image& Prototype)
	: CUIObject(Prototype)
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
		m_pDesc = new UIIMAGE_DESC(*static_cast<UIIMAGE_DESC*>(pArg));
	}
	else
		return E_FAIL;

	if (FAILED(__super::Initialize(m_pDesc)))
		return E_FAIL;
	
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUI_Image::Priority_Update(_float fTimeDelta)
{

}

void CUI_Image::Update(_float fTimeDelta)
{
	/*if (m_pDesc->eUIState == CLICKABLE && Is_KeyDown())
		int a = 0;*/
}

void CUI_Image::Late_Update(_float fTimeDelta)
{
	//m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);
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

_bool CUI_Image::is_Hovering()
{
	_float3		ptMouse = m_pGameInstance->Get_MousePos();
	POINT		MousePosition	{ static_cast<_long>(ptMouse.x), static_cast<_long>(ptMouse.y) };

	RECT		rcUI = { static_cast<_long>(m_pDesc->fX - m_pDesc->fSizeX * 0.5f),
						 static_cast<_long>(m_pDesc->fY - m_pDesc->fSizeY * 0.5f),
						 static_cast<_long>(m_pDesc->fX + m_pDesc->fSizeX * 0.5f),
						 static_cast<_long>(m_pDesc->fY + m_pDesc->fSizeY * 0.5f)
	};

	return PtInRect(&rcUI, MousePosition);
}

_bool CUI_Image::Is_KeyDown()
{
	return is_Hovering() && m_pGameInstance->Key_Down(VK_LBUTTON);
}

_bool CUI_Image::Is_KeyUp()
{
	return is_Hovering() && m_pGameInstance->Key_Up(VK_LBUTTON);
}

_bool CUI_Image::Is_KeyPressing()
{
	return is_Hovering() && m_pGameInstance->Key_Pressing(VK_LBUTTON);
}


HRESULT CUI_Image::Ready_Components()
{
	/* Com_Texture */
	if (FAILED(__super::Add_Component(m_pDesc->iLayerLevelIndex, m_pDesc->strTextureComTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(& m_pTextureCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(m_pDesc->iPrototypeLevelIndex, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	  
	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(m_pDesc->iPrototypeLevelIndex, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
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

	Safe_Delete(m_pDesc);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
