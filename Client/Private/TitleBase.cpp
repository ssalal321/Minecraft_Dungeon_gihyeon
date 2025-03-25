#include "TitleBase.h"

#include "GameInstance.h"

CTitleBase::CTitleBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject { pDevice, pContext }
{

}

CTitleBase::CTitleBase(const CTitleBase& Prototype)
	: CUIObject{ Prototype }
{

}

HRESULT CTitleBase::Initialize_Prototype()
{
	/* 외부 데이터베이스를 통해서 값을 채운다. */

	return S_OK;
}

HRESULT CTitleBase::Initialize(void* pArg)
{
	/* 원형의 데이터를 복제하여 사본을 만들고. */
	/* 추가적으로 필요한 데이터를 Arg로 받아와 실 사용하기위한 객체의 정보를 생성해준다. */
	if (nullptr != pArg)
	{
		TITLEBASE_DESC* pDesc = static_cast<TITLEBASE_DESC*>(pArg);

		m_fPlayTime		= pDesc->fPlayTime;
		pDesc->fX		= g_iWinSizeX * 0.5f;
		pDesc->fY		= g_iWinSizeY * 0.5f;
		pDesc->fSizeX	= g_iWinSizeX;
		pDesc->fSizeY	= g_iWinSizeY;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CTitleBase::Priority_Update(_float fTimeDelta)
{

}

void CTitleBase::Update(_float fTimeDelta)
{
	_bool		isClicked = {};

	if (GetKeyState(VK_LBUTTON) & 0x8000)
		isClicked = isHit(g_hWnd);

}

void CTitleBase::Late_Update(_float fTimeDelta)
{


	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);
}

HRESULT CTitleBase::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(__super::Bind_ShaderMatrices(m_pShaderCom, "g_ViewMatrix", "g_ProjMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;


	m_pVIBufferCom->Input_Assembler();
	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}


HRESULT CTitleBase::Ready_Components()
{
	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_TITLE, TEXT("Prototype_Component_Texture_TitleImage"),
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

CTitleBase* CTitleBase::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTitleBase* pGameInstance = new CTitleBase(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CTitleBase");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CGameObject* CTitleBase::Clone(void* pArg)
{
	CTitleBase* pGameInstance = new CTitleBase(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CTitleBase");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CTitleBase::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
