#include "HealthPotion.h"
#include "GameInstance.h"
#include "Player.h"

CHealthPotion::CHealthPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject(pDevice, pContext)
{
}

CHealthPotion::CHealthPotion(const CHealthPotion& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT CHealthPotion::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHealthPotion::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		m_pDesc = new HEALTHPOTION_ICON_DESC(*static_cast<HEALTHPOTION_ICON_DESC*>(pArg));
	}
	else
		return E_FAIL;

	if (FAILED(__super::Initialize(m_pDesc)))
		return E_FAIL;

	m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject(TEXT("GameObject_Player"), m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Player")));
	if (nullptr == m_pPlayer)
		return E_FAIL;

	m_pPlayerInfo = m_pPlayer->Get_PlayerInfo();
	if (nullptr == m_pPlayerInfo)
		return E_FAIL;

	if (FAILED(Ready_Components()))
			return E_FAIL;

	return S_OK;
}

void CHealthPotion::Priority_Update(_float fTimeDelta)
{

}

void CHealthPotion::Update(_float fTimeDelta)
{
	// 플레이어의 구르기 쿨타임 비율 가져오기
	_float fRemainTime = m_pPlayerInfo->Get_HPCooldown_RemainTime(); // 남은 쿨타임
	_float fTotalTime  = m_pPlayerInfo->Get_HPCooldown_TotalTime();   // 전체 쿨타임

	_float fRatio = fRemainTime / fTotalTime;
	fRatio = max(0.f, min(1.f, fRatio)); // Clamp(0~1)

	m_fCooldownRatio = fRatio;

	m_pShaderCom->Bind_RawValue("g_fHPCooldownRatio", &m_fCooldownRatio, sizeof(_float));
}


void CHealthPotion::Late_Update(_float fTimeDelta)
{
}

HRESULT CHealthPotion::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(Bind_ShaderMatrices(m_pShaderCom, "g_ViewMatrix", "g_ProjMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_IconTexture"))) // 이름 주의
		return E_FAIL;

	m_pVIBufferCom->Input_Assembler();
	m_pShaderCom->Begin(1); // CooldownIconTechnique의 pass index

	m_pVIBufferCom->Render();

	return S_OK;
}


HRESULT CHealthPotion::Ready_Components()
{
	/* Com_Texture*/
	if (nullptr == Add_Component(LEVEL_STATIC, m_pDesc->strTexPrototypeTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom)))
		return E_FAIL;

	/* Com_Shader */
	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex_CoolDownIcon"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

CHealthPotion* CHealthPotion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHealthPotion* pGameInstance = new CHealthPotion(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CHealthPotion");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject* CHealthPotion::Clone(void* pArg)
{
	CHealthPotion* pGameInstance = new CHealthPotion(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CHealthPotion");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CHealthPotion::Free()
{
	__super::Free();

	Safe_Delete(m_pDesc);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
