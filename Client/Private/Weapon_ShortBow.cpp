#include "Weapon_ShortBow.h"
#include "GameInstance.h"
#include "Item.h"

CWeapon_ShortBow::CWeapon_ShortBow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CItem(pDevice, pContext)
{

}

CWeapon_ShortBow::CWeapon_ShortBow(const CWeapon_ShortBow& Prototype)
	: CItem(Prototype)
{

}

HRESULT CWeapon_ShortBow::Initialize_Prototype()
{
	/* 외부 데이터베이스를 통해서 값을 채운다. */

	return S_OK;
}

HRESULT CWeapon_ShortBow::Initialize(void* pArg)
{
	/* 원형의 데이터를 복제하여 사본을 만들고. */
	/* 추가적으로 필요한 데이터를 Arg로 받아와 실 사용하기위한 객체의 정보를 생성해준다. */	

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_strGameObjectTag = TEXT("GameObject_ShortBow");
	m_strObjectPrototypeTag = TEXT("Prototype_GameObject_ShortBow");
	m_strTexPrototypeTag = TEXT("Prototype_Component_Texture_ShortBow");
	m_strIconGameObjectTag = TEXT("UIGameObject_ShortBow");
	m_strPartObjectTag = TEXT("Part_Weapon_Ranged");
	m_eItemtype = ITEM_TYPE::RANGED;
	m_iDealPoint = 8;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	// ShortBow 세팅
	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(270.f));
	return S_OK;
}

void CWeapon_ShortBow::Priority_Update(_float fTimeDelta)
{

}

void CWeapon_ShortBow::Update(_float fTimeDelta)
{

}

void CWeapon_ShortBow::Late_Update(_float fTimeDelta)
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

HRESULT CWeapon_ShortBow::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
		

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE, 0)))
			return E_FAIL;	

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(static_cast<_uint>(i))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CWeapon_ShortBow::Ready_Components()
{
	/* Com_Shader */
	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)))
		return E_FAIL;

	/* Com_Model */
	CModel::MODEL_DESC	pModelDesc = {};
	pModelDesc.bPickable = false;

	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_ShortBow"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &pModelDesc))
		return E_FAIL;

	///* Com_Collider */
	//CBounding_OBB::BOUNDING_OBB_DESC		OBBCollDesc{};

	//OBBCollDesc.vExtents = _float3(0.2f, 1.f, 0.2f);
	//OBBCollDesc.vCenter = _float3(0.f, OBBCollDesc.vExtents.y - 0.2f, 0.f);
	//OBBCollDesc.vRotation = _float3(0.f, /*XMConvertToRadians(0.f)*/ 0.f, 0.f);
	//OBBCollDesc.pGameObject = this;
	//OBBCollDesc.CombinedWorldMatrix = &m_CombinedWorldMatrix;
	//OBBCollDesc.pCollisionActivated = m_pBigCollisionActivating;

	//CComponent* pColliderCom = Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
	//	TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBCollDesc);

	//if (nullptr == pColliderCom)
	//	return E_FAIL;

	//m_pGameInstance->Add_ColliderCom(pColliderCom, TEXT("SkeletonWeapon_OBB"), TEXT("Monster"));

	return S_OK;
}


CWeapon_ShortBow* CWeapon_ShortBow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_ShortBow* pGameInstance = new CWeapon_ShortBow(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CWeapon_ShortBow");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CGameObject* CWeapon_ShortBow::Clone(void* pArg)
{
	CWeapon_ShortBow* pGameInstance = new CWeapon_ShortBow(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CWeapon_ShortBow");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CWeapon_ShortBow::Free()
{
	__super::Free();
}
