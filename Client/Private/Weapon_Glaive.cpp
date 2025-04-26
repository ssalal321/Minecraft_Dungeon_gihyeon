#include "Weapon_Glaive.h"
#include "GameInstance.h"
#include "Item.h"

CWeapon_Glaive::CWeapon_Glaive(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CItem(pDevice, pContext)
{

}

CWeapon_Glaive::CWeapon_Glaive(const CWeapon_Glaive& Prototype)
	: CItem(Prototype)
{

}

HRESULT CWeapon_Glaive::Initialize_Prototype()
{
	/* 외부 데이터베이스를 통해서 값을 채운다. */

	return S_OK;
}

HRESULT CWeapon_Glaive::Initialize(void* pArg)
{
	/* 원형의 데이터를 복제하여 사본을 만들고. */
	/* 추가적으로 필요한 데이터를 Arg로 받아와 실 사용하기위한 객체의 정보를 생성해준다. */	
	ITEM_DESC* pDesc = static_cast<ITEM_DESC*>(pArg);

	m_pTargetState = pDesc->pState;
	m_pSocketMatrix = pDesc->pSocketMatrix;
	m_eItemtype = ITEM_TYPE::MELEE;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	// Glaive 세팅
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(100.f));
	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.02f, -0.05f, 0.4f, 1.f));
	return S_OK;
}

void CWeapon_Glaive::Priority_Update(_float fTimeDelta)
{

}

void CWeapon_Glaive::Update(_float fTimeDelta)
{

}

void CWeapon_Glaive::Late_Update(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)	
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);	

	XMStoreFloat4x4(&m_CombinedWorldMatrix, 
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * 
		SocketMatrix *
		XMLoadFloat4x4(m_pParentWorldMatrix));

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CWeapon_Glaive::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
		

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;	

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CWeapon_Glaive::Ready_Components()
{
	/* Com_Shader */
	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)))
		return E_FAIL;

	/* Com_Model */
	CModel::MODEL_DESC	pModelDesc = {};
	pModelDesc.bPickable = false;

	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_GlaiveSteel"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &pModelDesc))
		return E_FAIL;

	/* Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC		OBBCollDesc{};

	OBBCollDesc.vExtents = _float3(0.2f, 1.f, 0.2f);
	OBBCollDesc.vCenter = _float3(0.f, OBBCollDesc.vExtents.y - 0.2f, 0.f);
	OBBCollDesc.vRotation = _float3(0.f, /*XMConvertToRadians(0.f)*/ 0.f, 0.f);
	OBBCollDesc.pGameObject = this;
	OBBCollDesc.CombinedWorldMatrix = &m_CombinedWorldMatrix;
	OBBCollDesc.pContainerObjAttacking = m_pContainerObjAttacking;

	CComponent* pColliderCom = Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBCollDesc);

	if (nullptr == pColliderCom)
		return E_FAIL;

	m_pGameInstance->Add_ColliderCom(pColliderCom, TEXT("PlayerWeapon_OBB"), TEXT("Player"));

	return S_OK;
}


CWeapon_Glaive* CWeapon_Glaive::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Glaive* pGameInstance = new CWeapon_Glaive(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CWeapon_Glaive");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CGameObject* CWeapon_Glaive::Clone(void* pArg)
{
	CWeapon_Glaive* pGameInstance = new CWeapon_Glaive(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CWeapon_Glaive");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CWeapon_Glaive::Free()
{
	__super::Free();

}
