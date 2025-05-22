#include "GateFence.h"
#include "GameInstance.h"

CGateFence::CGateFence(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CGateFence::CGateFence(const CGateFence& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CGateFence::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGateFence::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	GATEFENCE_DESC* pDesc = static_cast<GATEFENCE_DESC*>(pArg);
	m_WorldPosition		= pDesc->worldPosition;

	pDesc->fRotationPerSec = 90.f;
	pDesc->fSpeedPerSec = 0.f;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fAppearDuration = 3.f;
	m_fDeathDuration = 3.f;

	m_bActive = false;

	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(270.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_WorldPosition));

	return S_OK;
}

void CGateFence::Priority_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;
}

void CGateFence::Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	if (m_bAppearing)
	{
		m_fAppearTime += fTimeDelta;
		if (m_fAppearTime >= m_fAppearDuration)
		{
			m_bAppearing = false;
			m_fAppearTime = m_fAppearDuration;
		}
	}

	if (m_bDying)
	{
		m_fDeathTime += fTimeDelta;
		if (m_fDeathTime >= m_fDeathDuration)
		{
			m_bDying = false;
			m_fDeathTime = m_fDeathDuration;
			m_bActive = false;
		}
	}
}

void CGateFence::Late_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
	}

HRESULT CGateFence::Render()
{
	if (!m_bActive)
		return S_OK;

	_uint iPassIndex = {};
	if (m_bDying)
		iPassIndex = 4; // Disappear
	else if (m_bAppearing)
		iPassIndex = 3; // Appear
	else
		iPassIndex = 0; // Default

	if (FAILED(Bind_ShaderResources(iPassIndex)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(iPassIndex)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(static_cast<_uint>(i))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CGateFence::Bind_ShaderResources(_uint iPassIndex)
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	// ===== 패스별 바인딩 =====
	switch (iPassIndex)
	{
	case 3: // Appear
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAppearTime", &m_fAppearTime, sizeof(_float)))) return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAppearDuration", &m_fAppearDuration, sizeof(_float)))) return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bAppearing", &m_bAppearing, sizeof(_bool)))) return E_FAIL;
		break;

	case 4: // Disappear
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bDying", &m_bDying, sizeof(_bool)))) return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDeathTime", &m_fDeathTime, sizeof(_float)))) return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDeathDuration", &m_fDeathDuration, sizeof(_float)))) return E_FAIL;
		break;

	case 1: // Default
	default:
		// 아무것도 안 해도 됨 (g_bAppearing = false 상태가 기본)
		break;
	}

	return S_OK;
}


HRESULT CGateFence::Ready_Components()
{
	/* Com_Shader */
	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)))
		return E_FAIL;

	/* Com_Model */
	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_GateFence"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

CGateFence* CGateFence::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGateFence* pGameInstance = new CGateFence(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CGateFence");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject* CGateFence::Clone(void* pArg)
{
	CGateFence* pGameInstance = new CGateFence(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CGateFence");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CGateFence::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
