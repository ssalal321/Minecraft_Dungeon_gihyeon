#include "Slime_Cauldron_Bullet.h"

#include "BulletPool_Monster.h"
#include "GameInstance.h"

_int  CSlime_Cauldron_Bullet::m_iBulletID = 0;

CSlime_Cauldron_Bullet::CSlime_Cauldron_Bullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CSlime_Cauldron_Bullet::CSlime_Cauldron_Bullet(const CSlime_Cauldron_Bullet& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CSlime_Cauldron_Bullet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSlime_Cauldron_Bullet::Initialize(void* pArg)
{
	const _wstring& arrowGameObjectTag = TEXT("GameObject_SlimeCauldronBullet_") + to_wstring(m_iBulletID++);

	m_pBulletDesc = new GAMEOBJECT_DESC(arrowGameObjectTag, 0.f, 10.f);

	if (FAILED(__super::Initialize(m_pBulletDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->SetUp_Scale(0.15f, 0.15f, 0.15f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 0.f, 200.f, 0.f, 1.f });

	return S_OK;
}

void CSlime_Cauldron_Bullet::Priority_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;
}

void CSlime_Cauldron_Bullet::Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	if (m_bAttacking)
	{
		m_pTransformCom->Go_Straight(fTimeDelta);

		m_fResetTimer += fTimeDelta;
		if (m_fResetTimer >= 2.f)
			Return_To_Pool();
	}
		
	if (m_bCollided)
	{
		m_fResetTimer += fTimeDelta;

		if (m_fResetTimer >= 0.2f)
			Return_To_Pool();
	}
}

void CSlime_Cauldron_Bullet::Late_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CSlime_Cauldron_Bullet::Render()
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

HRESULT CSlime_Cauldron_Bullet::Bind_ShaderResources()
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

void CSlime_Cauldron_Bullet::Fire(_float4 startPos, _float4 lookPos)
{
	startPos.y += 1.f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&startPos));
	m_pTransformCom->LookAt(XMLoadFloat4(&lookPos));
	m_bAttacking	= true;
	m_bActive		= true;
	m_pColliderCom->Set_ColliderActive(true);
}

void CSlime_Cauldron_Bullet::Return_To_Pool()
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 0.f, 200.f, 0.f, 1.f });
	m_fResetTimer	= 0.f;
	m_iDealPoint	= 0;

	m_bAttacking	= false;
	m_bCollided		= false;
	m_bActive		= false;
	m_pColliderCom->Set_ColliderActive(false);

	if (m_pBulletPool)
		m_pBulletPool->Return_Bullet(this);
}

void CSlime_Cauldron_Bullet::Collided_With(CCollider* pOther, CCollider::COLLISION_STATE eCollisionState)
{
	if (pOther->Is_ColliderActive() &&
		CCollider::COLLISION_STATE::ENTER == eCollisionState &&
		TEXT("Player_Body_Small") == pOther->Get_ColliderTag())
	{
		m_bCollided = true;
	}
}

HRESULT CSlime_Cauldron_Bullet::Ready_Components()
{
	/* Com_Shader */
	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)))
		return E_FAIL;

	/* Com_Model */
	CModel::MODEL_DESC	pModelDesc = {};
	pModelDesc.bPickable = false;

	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Cube"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &pModelDesc))
		return E_FAIL;

	/*Com_Collider*/
	CBounding_Sphere::BOUNDING_SPHERE_DESC		SphereCollDesc{};

	SphereCollDesc.fRadius = 1.5f;
	SphereCollDesc.vCenter = _float3(0.f, 0.f, 0.f);
	SphereCollDesc.pGameObject = this;
	SphereCollDesc.CombinedWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	SphereCollDesc.pCollisionActivated = &m_bAttacking;

	CComponent* pColliderSphereCom = Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereCollDesc);

	if (nullptr == pColliderSphereCom)
		return E_FAIL;

	m_pGameInstance->Add_ColliderCom(m_pGameInstance->Get_ChangedLevelIndex(), pColliderSphereCom, TEXT("Slime_Cauldron_Bullet"), TEXT("Monster"));

	CCollider* pArrowCollider = dynamic_cast<CCollider*>(pColliderSphereCom);
	pArrowCollider->Set_ColliderActive(false);
	pArrowCollider->Set_ColliderRole(CCollider::ETC);

	return S_OK;
}

CSlime_Cauldron_Bullet* CSlime_Cauldron_Bullet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSlime_Cauldron_Bullet* pGameInstance = new CSlime_Cauldron_Bullet(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CSlime_Cauldron_Bullet");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CGameObject* CSlime_Cauldron_Bullet::Clone(void* pArg)
{
	CSlime_Cauldron_Bullet* pGameInstance = new CSlime_Cauldron_Bullet(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CSlime_Cauldron_Bullet");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


void CSlime_Cauldron_Bullet::Free()
{
	__super::Free();

	Safe_Delete(m_pBulletDesc);

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
