#include "Wolf_Armor.h"
#include "GameInstance.h"
#include "Player.h"
#include "WolfArmor_Body.h"
#include "WolfArmor_L_Arm.h"
#include "WolfArmor_L_Leg.h"
#include "WolfArmor_Mask.h"
#include "WolfArmor_R_Arm.h"
#include "WolfArmor_R_Leg.h"

CWolf_Armor::CWolf_Armor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CArmor(pDevice, pContext)
{
}

CWolf_Armor::CWolf_Armor(const CWolf_Armor& Prototype)
	: CArmor(Prototype)
{
}

HRESULT CWolf_Armor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWolf_Armor::Initialize(void* pArg)
{
	m_eItemtype = ITEM_TYPE::ARMOR;
	m_bItemActive = true;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	

	if (FAILED(Ready_Armor_PartObjects()))
		return E_FAIL;

	return S_OK;
}

void CWolf_Armor::Priority_Update(_float fTimeDelta)
{
	for (auto& Pair : m_ArmorPartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Priority_Update(fTimeDelta);
	}
}

void CWolf_Armor::Update(_float fTimeDelta)
{
	for (auto& Pair : m_ArmorPartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Update(fTimeDelta);
	}
}

void CWolf_Armor::Late_Update(_float fTimeDelta)
{
	for (auto& Pair : m_ArmorPartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Late_Update(fTimeDelta);
	}
}

HRESULT CWolf_Armor::Render()
{
	if (false == m_bItemActive)
		return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(static_cast<_uint>(0))))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(static_cast<_uint>(i))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CWolf_Armor::Ready_Armor_PartObjects()
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject(TEXT("GameObject_Player"), m_pGameInstance->Get_ChangedLevelIndex(), TEXT("Layer_Player")));
	CTransform* pTransformCom = dynamic_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));
	CModel* pBody = dynamic_cast<CModel*>(pPlayer->Find_Part_Component(TEXT("Part_Body"), TEXT("Com_Model")));
	if (nullptr == pBody)
		return E_FAIL;

#pragma region MASK
	CWolfArmor_Mask::WOLFARMOR_MASK_DESC	pMaskDesc{};
	pMaskDesc.pParentWorldMatrix = pTransformCom->Get_WorldMatrix_Ptr();
	pMaskDesc.pMaskSocketMatrix	 = pBody->Get_CombinedTransformationMatrix("Head_Armor");
	pMaskDesc.pContainerObject	 = pPlayer;
	pMaskDesc.bPartActive		 = m_bItemActive;

	if (FAILED(__super::Add_PartObject(LEVEL_STATIC, TEXT("Prototype_GameObject_WolfArmor_Mask"), TEXT("Part_Mask"), &pMaskDesc)))
		return E_FAIL;
#pragma endregion

#pragma region BODY
	CWolfArmor_Body::WOLFARMOR_BODY_DESC	pBodyDesc{};
	pBodyDesc.pParentWorldMatrix = pTransformCom->Get_WorldMatrix_Ptr();
	pBodyDesc.pBodySocketMatrix  = pBody->Get_CombinedTransformationMatrix("Body_armor");
	pBodyDesc.pContainerObject   = pPlayer;
	pBodyDesc.bPartActive		 = m_bItemActive;

	if (FAILED(__super::Add_PartObject(LEVEL_STATIC, TEXT("Prototype_GameObject_WolfArmor_Body"), TEXT("Part_Body"), &pBodyDesc)))
		return E_FAIL;
#pragma endregion

#pragma region L_ARM
	CWolfArmor_L_Arm::WOLFARMOR_L_ARM_DESC	 pLArmDesc{};
	pLArmDesc.pParentWorldMatrix = pTransformCom->Get_WorldMatrix_Ptr();
	pLArmDesc.pLArmSocketMatrix	 = pBody->Get_CombinedTransformationMatrix("L_Arm_armor");
	pLArmDesc.pContainerObject   = pPlayer;
	pLArmDesc.bPartActive		 = m_bItemActive;

	if (FAILED(__super::Add_PartObject(LEVEL_STATIC, TEXT("Prototype_GameObject_WolfArmor_L_Arm"), TEXT("Part_L_Arm"), &pLArmDesc)))
		return E_FAIL;
#pragma endregion

#pragma region R_ARM
	CWolfArmor_R_Arm::WOLFARMOR_R_ARM_DESC	 pRArmDesc{};
	pRArmDesc.pParentWorldMatrix = pTransformCom->Get_WorldMatrix_Ptr();
	pRArmDesc.pRArmSocketMatrix  = pBody->Get_CombinedTransformationMatrix("R_Arm_armor");
	pRArmDesc.pContainerObject   = pPlayer;
	pRArmDesc.bPartActive		 = m_bItemActive;

	if (FAILED(__super::Add_PartObject(LEVEL_STATIC, TEXT("Prototype_GameObject_WolfArmor_R_Arm"), TEXT("Part_R_Arm"), &pRArmDesc)))
		return E_FAIL;
#pragma endregion

#pragma region L_LEG
	CWolfArmor_L_Leg::WOLFARMOR_L_LEG_DESC	 pLLegDesc{};
	pLLegDesc.pParentWorldMatrix = pTransformCom->Get_WorldMatrix_Ptr();
	pLLegDesc.pLLegSocketMatrix  = pBody->Get_CombinedTransformationMatrix("L_Leg_armor");
	pLLegDesc.pContainerObject	 = pPlayer;
	pLLegDesc.bPartActive		 = m_bItemActive;

	if (FAILED(__super::Add_PartObject(LEVEL_STATIC, TEXT("Prototype_GameObject_WolfArmor_L_Leg"), TEXT("Part_L_Leg"), &pLLegDesc)))
		return E_FAIL;
#pragma endregion

#pragma region R_LEG
	CWolfArmor_R_Leg::WOLFARMOR_R_LEG_DESC	 pRLegDesc{};
	pRLegDesc.pParentWorldMatrix = pTransformCom->Get_WorldMatrix_Ptr();
	pRLegDesc.pRLegSocketMatrix = pBody->Get_CombinedTransformationMatrix("R_Leg_armor");
	pRLegDesc.pContainerObject = pPlayer;
	pRLegDesc.bPartActive = m_bItemActive;

	if (FAILED(__super::Add_PartObject(LEVEL_STATIC, TEXT("Prototype_GameObject_WolfArmor_R_Leg"), TEXT("Part_R_Leg"), &pRLegDesc)))
		return E_FAIL;
#pragma endregion

	return S_OK;
}


CWolf_Armor* CWolf_Armor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWolf_Armor* pGameInstance = new CWolf_Armor(pDevice, pContext);
	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CWolf_Armor");
		Safe_Release(pGameInstance);
	}
	return pGameInstance;
}

CGameObject* CWolf_Armor::Clone(void* pArg)
{
	CWolf_Armor* pGameInstance = new CWolf_Armor(*this);
	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CWolf_Armor");
		Safe_Release(pGameInstance);
	}
	return pGameInstance;
}

void CWolf_Armor::Free()
{
	__super::Free();
	
}
