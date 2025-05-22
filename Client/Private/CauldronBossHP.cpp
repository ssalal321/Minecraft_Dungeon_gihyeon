#include "CauldronBossHP.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "CauldronBoss.h"

CCauldronBossHP::CCauldronBossHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject(pDevice, pContext)
{
}

CCauldronBossHP::CCauldronBossHP(const CCauldronBossHP& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT CCauldronBossHP::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCauldronBossHP::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		m_pDesc = new CAULDRONBOSS_HP_DESC(*static_cast<CAULDRONBOSS_HP_DESC*>(pArg));
	}
	else
		return E_FAIL;

	if (FAILED(__super::Initialize(m_pDesc)))
		return E_FAIL;

	m_pCauldronBoss = m_pDesc->pBoss;
	m_pBody_CauldronBoss = m_pCauldronBoss->Find_PartObject(TEXT("Part_Body"));
	m_pMonsterDesc = m_pCauldronBoss->Get_MonsterInfo();

	_uint uiMaxHP = m_pMonsterDesc->Get_MaxHP();
	m_fInverseMaxHP = 1.f / static_cast<_float>(uiMaxHP);

	if (FAILED(Ready_CauldronBossHP_Components()))
			return E_FAIL;

	m_bActive = false;


	m_vScreenPos = { m_pDesc->fX - 53.f, m_pDesc->fY - 45.f };
	/*XMStoreFloat2(&m_vScreenPos,
		XMVector3Project(
			m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, -10.f, 0.f, 1.f),
			0.f,
			0.f,
			static_cast<_float>(g_iWinSizeX),
			static_cast<_float>(g_iWinSizeY),
			0.f,
			1.f,
			m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ),
			m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW),
			XMMatrixIdentity()));*/

	return S_OK;
}

void CCauldronBossHP::Priority_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;
}

void CCauldronBossHP::Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	_uint uiCurrentHP = m_pMonsterDesc->Get_CurrentHP();
	m_CutOffX = static_cast<_float>(uiCurrentHP) * m_fInverseMaxHP;  // 0~1 범위

	if (m_CutOffX < 0.f) m_CutOffX = 0.f;

	m_pShaderCom->Bind_RawValue("g_fCutoffX", &m_CutOffX, sizeof(_float));
}


void CCauldronBossHP::Late_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;
}

HRESULT CCauldronBossHP::Render()
{
	if (!m_bActive)
		return S_OK;

	m_pGameInstance->Draw_Text(TEXT("Font_Interop"), TEXT("타락한 가마솥"), m_vScreenPos, Colors::White, 0.f, { 0.f, 0.f }, 0.5f);

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(Bind_ShaderMatrices(m_pShaderCom, "g_ViewMatrix", "g_ProjMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_HPTexture")))
		return E_FAIL;

	m_pVIBufferCom->Input_Assembler();
	m_pShaderCom->Begin(1); // BossHP Pass Index

	m_pVIBufferCom->Render();
	return S_OK;
}


HRESULT CCauldronBossHP::Ready_CauldronBossHP_Components()
{
	if (nullptr == Add_Component(LEVEL_STATIC, m_pDesc->strTexPrototypeTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom)))
		return E_FAIL;

	/* Com_Shader */
	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex_HPbar"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

CCauldronBossHP* CCauldronBossHP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCauldronBossHP* pGameInstance = new CCauldronBossHP(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CCauldronBossHP");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject* CCauldronBossHP::Clone(void* pArg)
{
	CCauldronBossHP* pGameInstance = new CCauldronBossHP(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CCauldronBossHP");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CCauldronBossHP::Free()
{
	__super::Free();

	Safe_Delete(m_pDesc);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
