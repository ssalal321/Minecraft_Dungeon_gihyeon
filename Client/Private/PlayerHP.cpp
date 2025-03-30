#include "PlayerHP.h"
#include "GameInstance.h"

CPlayerHP::CPlayerHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject(pDevice, pContext)
{
}

CPlayerHP::CPlayerHP(const CPlayerHP& Prototype)
	: CUIObject(Prototype), m_CutOffY(Prototype.m_CutOffY)
{
}

HRESULT CPlayerHP::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayerHP::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		m_pDesc = new PLAYERHP_DESC(*static_cast<PLAYERHP_DESC*>(pArg));
	}
	else
		return E_FAIL;

	if (FAILED(__super::Initialize(m_pDesc)))
		return E_FAIL;

	
	if (FAILED(Ready_PlayerHP_Components()))
			return E_FAIL;

	return S_OK;
}

void CPlayerHP::Priority_Update(_float fTimeDelta)
{

}

void CPlayerHP::Update(_float fTimeDelta)
{
	if (Get_KeyDown())
	{
		m_CutOffY += 0.02f;

		if (m_CutOffY > 1.0f) m_CutOffY = 1.0f; // 최대값 제한

		m_pShaderCom->Bind_RawValue("g_fCutoffY", &m_CutOffY, sizeof(float));


		_float fYFactor = (m_CutOffY < 0.75f) ? 2.f : 5.f;
		m_pShaderCom->Bind_RawValue("g_fYGradationFactor", &fYFactor, sizeof(float));
		
	}
}

void CPlayerHP::Late_Update(_float fTimeDelta)
{
}

HRESULT CPlayerHP::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(Bind_ShaderMatrices(m_pShaderCom, "g_ViewMatrix", "g_ProjMatrix")))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_HPTexture")))
			return E_FAIL;

	m_pVIBufferCom->Input_Assembler();
	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CPlayerHP::Ready_PlayerHP_Components()
{
	/* Com_Texture - filled_heart 텍스처 */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_pDesc->strTextureComTag,
		TEXT("Com_Texture_PlayerHP"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex_HPbar"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

_bool CPlayerHP::Get_KeyDown()
{
	return m_pGameInstance->Get_Key(VK_NUMPAD1);
}

CPlayerHP* CPlayerHP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayerHP* pGameInstance = new CPlayerHP(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CPlayerHP");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject* CPlayerHP::Clone(void* pArg)
{
	CPlayerHP* pGameInstance = new CPlayerHP(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CPlayerHP");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CPlayerHP::Free()
{
	__super::Free();

	Safe_Delete(m_pDesc);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
