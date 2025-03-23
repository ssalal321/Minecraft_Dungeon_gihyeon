#include "HP_bar.h"
#include "Client_Defines.h"
#include "GameInstance.h"

CHP_Bar::CHP_Bar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject(pDevice, pContext)
{
}

CHP_Bar::CHP_Bar(const CHP_Bar& Prototype)
	: CUIObject(Prototype), m_CutOffY(Prototype.m_CutOffY)
{
}

HRESULT CHP_Bar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHP_Bar::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		m_pDesc = new UI_HPBAR_DESC(*static_cast<UI_HPBAR_DESC*>(pArg));
	}
	else
		return E_FAIL;

	if (FAILED(__super::Initialize(m_pDesc)))
		return E_FAIL;

	switch (m_pDesc->eHPbarType)
	{
	case PLAYERHP:
		if (FAILED(Ready_PlayerHP_Components()))
			return E_FAIL;
		break;
	}

	return S_OK;
}

void CHP_Bar::Priority_Update(_float fTimeDelta)
{

}

void CHP_Bar::Update(_float fTimeDelta)
{
	if (Get_KeyDown())
	{
		m_CutOffY += 0.02f;

		if (m_CutOffY > 1.0f) m_CutOffY = 1.0f; // 최대값 제한

		m_pShaderCom->Bind_RawValue("g_fCutoffY", &m_CutOffY, sizeof(float));
		
	}
}

void CHP_Bar::Last_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);
}

HRESULT CHP_Bar::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(Bind_ShaderMatrices(m_pShaderCom, "g_ViewMatrix", "g_ProjMatrix")))
		return E_FAIL;


	switch (m_pDesc->eHPbarType)
	{
	case PLAYERHP:
		// filled_heart 텍스처 바인딩
		if (FAILED(m_pTextureCom_PlayerHP->Bind_ShaderResource(m_pShaderCom, "g_HPTexture")))
			return E_FAIL;
		break;
	}

	m_pVIBufferCom->Input_Assembler();

	switch (m_pDesc->eHPbarType)
	{
	case PLAYERHP:
		m_pShaderCom->Begin(0);
		break;

	//case BOSS:
	//	m_pShaderCom->Begin("BossHP");
	//	break;
	//case ENEMY:
	//	m_pShaderCom->Begin("EnemyHP");
	//	break;
	}

	
	//m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;

}

HRESULT CHP_Bar::Ready_PlayerHP_Components()
{
	/* Com_Texture - filled_heart 텍스처 */
	if (FAILED(__super::Add_Component(m_pDesc->iLayerLevelIndex, m_pDesc->strTextureComTag,
		TEXT("Com_Texture_PlayerHP"), reinterpret_cast<CComponent**>(&m_pTextureCom_PlayerHP))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(m_pDesc->iLayerLevelIndex, TEXT("Prototype_Component_Shader_VtxPosTex_HPbar"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(m_pDesc->iPrototypeLevelIndex, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}


_bool CHP_Bar::is_Hit_for_Test()
{
	_float3		ptMouse = m_pGameInstance->Get_MousePos();
	POINT		MousePosition{ static_cast<_long>(ptMouse.x), static_cast<_long>(ptMouse.y) };

	RECT		rcUI = { static_cast<_long>(m_pDesc->fX - m_pDesc->fSizeX * 0.5f),
		static_cast<_long>(m_pDesc->fY - m_pDesc->fSizeY * 0.5f),
		static_cast<_long>(m_pDesc->fX + m_pDesc->fSizeX * 0.5f),
		static_cast<_long>(m_pDesc->fY + m_pDesc->fSizeY * 0.5f)
	};

	return PtInRect(&rcUI, MousePosition);
}

_bool CHP_Bar::Get_KeyDown()
{
	return m_pGameInstance->Get_Key(VK_NUMPAD1);
}


CHP_Bar* CHP_Bar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHP_Bar* pGameInstance = new CHP_Bar(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CHP_Bar");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject* CHP_Bar::Clone(void* pArg)
{
	CHP_Bar* pGameInstance = new CHP_Bar(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CHP_Bar");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CHP_Bar::Free()
{
	__super::Free();

	Safe_Delete(m_pDesc);
	Safe_Release(m_pTextureCom_PlayerHP);
	Safe_Release(m_pTextureCom_PlayerHP_Top);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
