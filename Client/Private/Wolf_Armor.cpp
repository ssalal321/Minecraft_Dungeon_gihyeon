#include "Wolf_Armor.h"
#include "GameInstance.h"

CWolf_Armor::CWolf_Armor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CArmor(pDevice, pContext)
{
	ZeroMemory(m_pTransformCom, sizeof(m_pTransformCom));
	ZeroMemory(m_pModelCom, sizeof(m_pModelCom));
	ZeroMemory(m_pSocketMatrix, sizeof(m_pSocketMatrix));
	ZeroMemory(m_CombinedWorldMatrix, sizeof(m_CombinedWorldMatrix));
}

CWolf_Armor::CWolf_Armor(const CWolf_Armor& Prototype)
	: CArmor(Prototype)
{
	memcpy(m_pTransformCom, Prototype.m_pTransformCom, sizeof(m_pTransformCom));
	memcpy(m_pModelCom, Prototype.m_pModelCom, sizeof(m_pModelCom));
	memcpy(m_pSocketMatrix, Prototype.m_pSocketMatrix, sizeof(m_pSocketMatrix));
	memcpy(m_CombinedWorldMatrix, Prototype.m_CombinedWorldMatrix, sizeof(m_CombinedWorldMatrix));
}

HRESULT CWolf_Armor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWolf_Armor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_strGameObjectTag = TEXT("GameObject_Wolf_Armor");
	m_strObjectPrototypeTag = TEXT("Prototype_GameObject_Wolf_Armor");
	m_strTexPrototypeTag = TEXT("Prototype_Component_Texture_Wolf_Armor");
	m_strIconGameObjectTag = TEXT("UIGameObject_Wolf_Armor");

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CWolf_Armor::Priority_Update(_float fTimeDelta) {}
void CWolf_Armor::Update(_float fTimeDelta) {}

void CWolf_Armor::Late_Update(_float fTimeDelta)
{
	if (!m_bItemActive) return;

	for (_uint i = 0; i < PART_END; ++i)
	{
		if (!m_pTransformCom[i] || !m_pModelCom[i] || !m_pSocketMatrix[i])
			continue;

		_matrix socketMat = XMLoadFloat4x4(m_pSocketMatrix[i]);
		for (_int j = 0; j < 3; ++j)
			socketMat.r[j] = XMVector3Normalize(socketMat.r[j]);

		_matrix combinedMat = XMLoadFloat4x4(m_pTransformCom[i]->Get_WorldMatrix_Ptr()) *
			socketMat *
			XMLoadFloat4x4(m_pParentWorldMatrix);

		XMStoreFloat4x4(&m_CombinedWorldMatrix[i], combinedMat);

		m_iRenderingPartIndex = i;
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	}
}

HRESULT CWolf_Armor::Render()
{
	if (!m_bItemActive || m_iRenderingPartIndex >= PART_END)
		return S_OK;

	if (FAILED(Bind_ShaderResources(m_iRenderingPartIndex)))
		return E_FAIL;

	CModel* pModel = m_pModelCom[m_iRenderingPartIndex];
	if (!pModel) return E_FAIL;

	_uint iNumMeshes = pModel->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(pModel->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(pModel->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CWolf_Armor::Bind_ShaderResources(_uint iPartIndex)
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix[iPartIndex])))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if (!pLightDesc) return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CWolf_Armor::Ready_Components()
{
	__super::Ready_Components();

	CModel::MODEL_DESC pModelDesc = { false };
	CTransform::TRANSFORM_DESC tDesc{ 0.0f, XMConvertToRadians(90.f) };

	static const std::wstring partTags[PART_END] = {
		TEXT("Com_Mask_Model"), TEXT("Com_Body_Model"), TEXT("Com_L_Arm_Model"),
		TEXT("Com_R_Arm_Model"), TEXT("Com_L_Leg_Model"), TEXT("Com_R_Leg_Model")
	};

	static const std::wstring protoTags[PART_END] = {
		TEXT("Prototype_Component_Model_Wolf_Armor_Mask"),
		TEXT("Prototype_Component_Model_Wolf_Armor_FurArmor"),
		TEXT("Prototype_Component_Model_Wolf_Armor_L_Arm"),
		TEXT("Prototype_Component_Model_Wolf_Armor_R_Arm"),
		TEXT("Prototype_Component_Model_Wolf_Armor_L_Leg"),
		TEXT("Prototype_Component_Model_Wolf_Armor_R_Leg")
	};

	for (_uint i = 0; i < PART_END; ++i)
	{
		if (nullptr == Add_Component(LEVEL_STATIC, protoTags[i], partTags[i], reinterpret_cast<CComponent**>(&m_pModelCom[i]), &pModelDesc))
			return E_FAIL;

		m_pTransformCom[i] = CTransform::Create(m_pDevice, m_pContext);
		if (!m_pTransformCom[i]) return E_FAIL;
		if (FAILED(m_pTransformCom[i]->Initialize(&tDesc))) return E_FAIL;
		m_pTransformCom[i]->SetUp_Scale(1.f, 1.f, 1.f);
	}

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
	for (_uint i = 0; i < PART_END; ++i)
	{
		Safe_Release(m_pModelCom[i]);
		Safe_Release(m_pTransformCom[i]);
	}
}
