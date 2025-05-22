#include "RollIcon.h"
#include "GameInstance.h"
#include "Player.h"

CRollIcon::CRollIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject(pDevice, pContext)
{
}

CRollIcon::CRollIcon(const CRollIcon& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT CRollIcon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRollIcon::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		m_pDesc = new ROLL_ICON_DESC(*static_cast<ROLL_ICON_DESC*>(pArg));
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

	_uint  uiMaxHP = m_pPlayerInfo->Get_MaxHP();
	m_fInverseMaxHP = 1.f / static_cast<_float>(uiMaxHP);

	if (FAILED(Ready_Components()))
			return E_FAIL;

	return S_OK;
}

void CRollIcon::Priority_Update(_float fTimeDelta)
{

}

void CRollIcon::Update(_float fTimeDelta)
{
	// 플레이어의 구르기 쿨타임 비율 가져오기
	_float fRemainTime = m_pPlayerInfo->Get_RollCooldown_RemainTime(); // 남은 쿨타임
	_float fTotalTime  = m_pPlayerInfo->Get_RollCooldown_TotalTime();   // 전체 쿨타임

	_float fRatio = fRemainTime / fTotalTime;
	fRatio = max(0.f, min(1.f, fRatio)); // Clamp(0~1)

	m_fCooldownRatio = fRatio;

	m_pShaderCom->Bind_RawValue("g_fCooldownRatio", &m_fCooldownRatio, sizeof(_float));
}


void CRollIcon::Late_Update(_float fTimeDelta)
{
}

HRESULT CRollIcon::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(Bind_ShaderMatrices(m_pShaderCom, "g_ViewMatrix", "g_ProjMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_IconTexture"))) // 이름 주의
		return E_FAIL;

	m_pVIBufferCom->Input_Assembler();
	m_pShaderCom->Begin(0); // CooldownIconTechnique의 pass index

	m_pVIBufferCom->Render();

	return S_OK;
}


HRESULT CRollIcon::Ready_Components()
{
	/* Com_Texture - filled_heart 텍스처 */
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

CRollIcon* CRollIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRollIcon* pGameInstance = new CRollIcon(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CRollIcon");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject* CRollIcon::Clone(void* pArg)
{
	CRollIcon* pGameInstance = new CRollIcon(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CRollIcon");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CRollIcon::Free()
{
	__super::Free();

	Safe_Delete(m_pDesc);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
