#include "Player_Arrow.h"

#include "ArrowPool_Player.h"
#include "GameInstance.h"

_int  CPlayer_Arrow::m_iArrowID = 0;

CPlayer_Arrow::CPlayer_Arrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer_Arrow::CPlayer_Arrow(const CPlayer_Arrow& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CPlayer_Arrow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_Arrow::Initialize(void* pArg)
{
	const _wstring& arrowGameObjectTag = TEXT("GameObject_PlayerArrow_") + to_wstring(m_iArrowID++);

	m_pArrowDesc = new GAMEOBJECT_DESC(arrowGameObjectTag, 0.f, 22.f);

	if (FAILED(__super::Initialize(m_pArrowDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 0.f, 200.f, 0.f, 1.f });

	return S_OK;
}

void CPlayer_Arrow::Priority_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;
}

void CPlayer_Arrow::Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	if (m_bColliderActivating)
	{
		m_pTransformCom->Go_Straight(fTimeDelta);

		m_fResetTimer += fTimeDelta;
		if (m_fResetTimer >= 3.5f)
			Return_To_Pool();
	}
		
	if (m_bCollided)
	{
		m_fResetTimer += fTimeDelta;

		if (m_fResetTimer >= 0.2f)
			Return_To_Pool();
	}
}

void CPlayer_Arrow::Late_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPlayer_Arrow::Render()
{
	if (!m_bActive)
		return S_OK;

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

HRESULT CPlayer_Arrow::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Arrow::Shoot(_float4 startPos, _float4 lookPos)
{
	startPos.y += 1.f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&startPos));
	m_pTransformCom->LookAt(XMLoadFloat4(&lookPos));
	m_bColliderActivating = true;
	m_bActive	 = true;
	m_pColliderCom->Set_ColliderActive(true);
}

void CPlayer_Arrow::Return_To_Pool()
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 0.f, 200.f, 0.f, 1.f});
	//m_pTransformCom->LookAt({ 0.f, 0.f, 0.f, 1.f });

	m_fResetTimer = 0.f;
	m_iDealPoint = 0;
	m_bColliderActivating = false;
	m_bCollided  = false;
	m_bActive	 = false;
	m_pColliderCom->Set_ColliderActive(false);

	if (m_pArrowPool)
		m_pArrowPool->Return_Arrow(this);
}

void CPlayer_Arrow::Collided_With(CCollider* pOther, CCollider::COLLISION_STATE eCollisionState)
{
	if (pOther->Get_ColliderActive() &&
		CCollider::COLLISION_STATE::ENTER == eCollisionState &&
		(TEXT("Monster_Body_Small") == pOther->Get_ColliderTag()))
	{
		m_bCollided = true;
		m_fResetTimer = 0.f;
	}
}

HRESULT CPlayer_Arrow::Ready_Components()
{
	/* Com_Shader */
	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)))
		return E_FAIL;

	/* Com_Model */
	CModel::MODEL_DESC	pModelDesc = {};
	pModelDesc.bPickable = false;

	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Arrow"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &pModelDesc))
		return E_FAIL;

	/* Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC		OBBCollDesc{};

	OBBCollDesc.vExtents  = _float3(0.2f, 0.2f, 0.6f);
	OBBCollDesc.vCenter   = _float3(0.f, OBBCollDesc.vExtents.y, 0.f);
	OBBCollDesc.vRotation = _float3(0.f, /*XMConvertToRadians(0.f)*/ 0.f, 0.f); 
	OBBCollDesc.pGameObject = this;
	OBBCollDesc.CombinedWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	OBBCollDesc.pCollisionActivated = &m_bColliderActivating;

	CComponent* pColliderCom = Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBCollDesc);

	if (nullptr == pColliderCom)
		return E_FAIL;

	m_pGameInstance->Add_ColliderCom(m_pGameInstance->Get_ChangedLevelIndex(), pColliderCom, TEXT("Player_Arrow"), TEXT("Player"), true);

	CCollider* pArrowCollider = dynamic_cast<CCollider*>(pColliderCom);
	pArrowCollider->Set_ColliderActive(false);
	pArrowCollider->Set_ColliderRole(CCollider::ETC);

	return S_OK;
}

CPlayer_Arrow* CPlayer_Arrow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Arrow* pGameInstance = new CPlayer_Arrow(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CPlayer_Arrow");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CGameObject* CPlayer_Arrow::Clone(void* pArg)
{
	CPlayer_Arrow* pGameInstance = new CPlayer_Arrow(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CPlayer_Arrow");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


void CPlayer_Arrow::Free()
{
	__super::Free();

	Safe_Delete(m_pArrowDesc);

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
