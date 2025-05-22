#include "Item.h"
#include "GameInstance.h"

_int CItem::m_iItemID = 0;

CItem::CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{

}

CItem::CItem(const CItem& Prototype)
	: CPartObject(Prototype)
{

}

HRESULT CItem::Initialize_Prototype()
{
	/* 외부 데이터베이스를 통해서 값을 채운다. */

	return S_OK;
}

HRESULT CItem::Initialize(void* pArg)
{
	/* 원형의 데이터를 복제하여 사본을 만들고. */
	/* 추가적으로 필요한 데이터를 Arg로 받아와 실 사용하기위한 객체의 정보를 생성해준다. */	
	ITEM_DESC* pDesc = static_cast<ITEM_DESC*>(pArg);

	if (nullptr != pDesc)
	{
		m_pTargetState = pDesc->pState;
		m_pSocketMatrix = pDesc->pSocketMatrix;
		m_strObjectPrototypeTag = pDesc->strObjectPrototypeTag;
		m_strTexPrototypeTag	= pDesc->strIconTexPrototypeTag;
		m_strIconGameObjectTag	= pDesc->strIconGameObjectTag;
		m_strPartObjectTag		= pDesc->strPartObjectTag;
		m_eItemtype				= pDesc->eItemtype;
		m_iDealPoint			= pDesc->iDealPoint;
		m_iInitialDealPoint		= pDesc->iDealPoint;
		m_iHealPoint			= pDesc->iHealPoint;
	}
	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CItem::Priority_Update(_float fTimeDelta)
{
}

void CItem::Update(_float fTimeDelta)
{
}

void CItem::Late_Update(_float fTimeDelta)
{
}

HRESULT CItem::Render()
{
	return S_OK;
}

HRESULT CItem::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
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

	return S_OK;
}


void CItem::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
