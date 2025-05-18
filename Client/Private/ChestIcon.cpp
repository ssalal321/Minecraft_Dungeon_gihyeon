#include "ChestIcon.h"
#include "GameInstance.h"
#include "InventoryData.h"
#include "InventorySlot.h"
#include "Player.h"

CChestIcon::CChestIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

HRESULT CChestIcon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CChestIcon::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		m_pDesc = new CHEST_ICON_DESC(*static_cast<CHEST_ICON_DESC*>(pArg));
	}
	else
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->SetUp_Scale(1.5f, 1.5f, 1.5f);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_pDesc->worldPosition));

	return S_OK;
}

void CChestIcon::Priority_Update(_float fTimeDelta)
{
	if (!m_bActivated)
		return;
}

void CChestIcon::Update(_float fTimeDelta)
{
	if (!m_bActivated)
		return;

	_float4     fWorldMousePos = {};
	_float3     fWorldMouseRay = {};
	m_pGameInstance->Compute_MouseRay(fWorldMousePos, fWorldMouseRay);

	CBounding_Sphere::RayDesc rayDesc = {};
	rayDesc.MousePos = { fWorldMousePos.x, fWorldMousePos.y, fWorldMousePos.z };
	rayDesc.MouseRay = fWorldMouseRay;
	//rayDesc.fDist = &fDist;

	if (m_pColliderCom->Get_Bounding()->Intersect(COLLIDER_TYPE::TYPE_RAY, nullptr, &rayDesc) &&
		m_pGameInstance->Key_Up(VK_LBUTTON) && !bMouseClickLock)
	{
		// 플레이어한테 끌려가기 On!

		// 먹혔을 때

		CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject(TEXT("GameObject_Player"), m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Player")));
		pPlayer->Get_InventoryData()->Add_Item_To_StoreSlot(m_pDesc->iPrototypeLevelIndex, m_pDesc->ItemDesc.strObjectPrototypeTag, m_pDesc->ItemDesc.strGameObjectTag, &m_pDesc->ItemDesc);

		m_bActivated = false;
	}

	// 뽀용 업데이트 중
	m_pTransformCom->Update_BezierFlight(fTimeDelta);
}

void CChestIcon::Late_Update(_float fTimeDelta)
{
	if (!m_bActivated)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CChestIcon::Render()
{
	if (!m_bActivated)
		return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	m_pVIBufferCom->Input_Assembler();
	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CChestIcon::Ready_Components()
{
	/* Com_Texture */
	if (nullptr == Add_Component(LEVEL_STATIC, m_pDesc->ItemDesc.strIconTexPrototypeTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom)))
		return E_FAIL;

	/* Com_Shader */
	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom)))
		return E_FAIL;

	XMStoreFloat4x4(&m_IdentityWorldMatrix, XMMatrixIdentity());
	/* Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC		SphereCollDesc{};

	SphereCollDesc.fRadius = 0.3f;
	SphereCollDesc.vCenter = _float3(0.f, SphereCollDesc.fRadius, 0.f);
	SphereCollDesc.pGameObject = this;
	SphereCollDesc.CombinedWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();;
	SphereCollDesc.pCollisionActivated = &m_bActivated;

	CComponent* pColliderSphereCom = Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereCollDesc);

	if (nullptr == pColliderSphereCom)
		return E_FAIL;

	m_pGameInstance->Add_ColliderCom(m_pGameInstance->Get_ChangedLevelIndex(), pColliderSphereCom, TEXT("Level_Change_Collider"), TEXT("Level"));
	dynamic_cast<CCollider*>(pColliderSphereCom)->Set_ColliderRole(CCollider::ETC);
	dynamic_cast<CCollider*>(pColliderSphereCom)->Set_MouseCollider(true);

	return S_OK;
}

HRESULT CChestIcon::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrix_Ptr())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	/*if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;*/

	return S_OK;
}

CChestIcon* CChestIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CChestIcon* pGameInstance = new CChestIcon(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : CChestIcon");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject* CChestIcon::Clone(void* pArg)
{
	CChestIcon* pGameInstance = new CChestIcon(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CChestIcon");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CChestIcon::Free()
{
	__super::Free();

	Safe_Delete(m_pDesc);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVIBufferCom);
}