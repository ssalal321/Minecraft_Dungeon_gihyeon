#include "LobbyChest.h"

#include <random>

#include "GameInstance.h"

CLobbyChest::CLobbyChest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CLobbyChest::CLobbyChest(const CLobbyChest& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CLobbyChest::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLobbyChest::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	LOBBY_CHEST_DESC* pDesc = static_cast<LOBBY_CHEST_DESC*>(pArg);
	m_WorldPosition		= pDesc->worldPosition;
	m_pChestIconDescs	= move(pDesc->pChestIconDescs);
	m_uiMoneyNum		= pDesc->uiMoneyNum;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(270.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_WorldPosition));

	return S_OK;
}

void CLobbyChest::Priority_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	for (auto& chestIcon : m_pChestIcons)
	{
		if (chestIcon)
			chestIcon->Priority_Update(fTimeDelta);
	}
}

void CLobbyChest::Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	_float4     fWorldMousePos = {};
	_float3     fWorldMouseRay = {};
	m_pGameInstance->Compute_MouseRay(fWorldMousePos, fWorldMouseRay);

	CBounding_Sphere::RayDesc rayDesc = {};
	rayDesc.MousePos = { fWorldMousePos.x, fWorldMousePos.y, fWorldMousePos.z };
	rayDesc.MouseRay = fWorldMouseRay;
	//rayDesc.fDist = &fDist;

	if (false == m_bOpened && m_pGameInstance->Key_Up(VK_LBUTTON) && !bMouseClickLock &&
		m_pColliderCom->Get_Bounding()->Intersect(COLLIDER_TYPE::TYPE_RAY, nullptr, &rayDesc))  // 플레이어와 일정 거리 이상이어야 추가도!
	{
		m_bOpened = true;

		Pop_Out_Items();
	}

	for (auto& chestIcon : m_pChestIcons)
	{
		if (chestIcon)
			chestIcon->Update(fTimeDelta);
	}
}

void CLobbyChest::Late_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);


	for (auto& chestIcon : m_pChestIcons)
	{
		if (chestIcon)
			chestIcon->Late_Update(fTimeDelta);
	}
}

HRESULT CLobbyChest::Render()
{
	if (!m_bActive)
		return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (m_bOpened)
	{
		_uint	iNumMeshes = m_pOpenedModelCom->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pOpenedModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE, 0)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(2)))
				return E_FAIL;

			if (FAILED(m_pOpenedModelCom->Render(static_cast<_uint>(i))))
				return E_FAIL;
		}

		return S_OK;
	}

	_uint	iNumMeshes = m_pClosedModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pClosedModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(2)))
			return E_FAIL;

		if (FAILED(m_pClosedModelCom->Render(static_cast<_uint>(i))))
			return E_FAIL;
	}

	for (auto& chestIcon : m_pChestIcons)
	{
		if (chestIcon)
			chestIcon->Render();
	}

	return S_OK;
}

HRESULT CLobbyChest::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix())))
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


void CLobbyChest::Pop_Out_Items()
{
	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_real_distribution<_float> distHeight(3.5f, 5.f);   // 높이
	std::uniform_real_distribution<_float> distSpeed(1.0f, 1.5f);   // 속도
	std::uniform_real_distribution<_float> distRandom(0.0f, 0.3f);  // 미세 흔들림

	const _uint iconCount = static_cast<_uint>(m_pChestIconDescs.size());

	for (_uint i = 0; i < iconCount; ++i)
	{
		auto& iconDesc = m_pChestIconDescs[i];

		CChestIcon* pIcon = CChestIcon::Create(m_pDevice, m_pContext, &iconDesc);
		if (!pIcon)
			continue;

		m_pChestIcons.push_back(pIcon);

		// 시작 위치: 상자 위
		XMFLOAT3 vStart = {
			m_WorldPosition.x,
			m_WorldPosition.y + 1.0f,
			m_WorldPosition.z
		};

		// 각도 계산 (아이콘 개수에 따라 분산)
		_float angleDeg = 60.f + (i * 60.f);  // 60, 120, 180, ...
		_float angleRad = XMConvertToRadians(angleDeg);

		// 기본 거리
		_float distanceX = -1.5f;  // 왼쪽으로 기본 거리 유지
		_float distanceZ = 1.5f;   // 앞으로 기본 거리 유지

		// 방향 벡터 계산 (회전된 방향)
		_float offsetX = cosf(angleRad) * distanceX + distRandom(gen);
		_float offsetZ = sinf(angleRad) * distanceZ + distRandom(gen);

		XMFLOAT3 vEnd = {
			vStart.x + offsetX,
			m_WorldPosition.y + 2.0f,
			vStart.z + offsetZ
		};

		XMFLOAT3 vControl = {
			(vStart.x + vEnd.x) * 0.5f,
			vStart.y + distHeight(gen),
			(vStart.z + vEnd.z) * 0.5f
		};

		_float fBezierSpeed = distSpeed(gen);

		CTransform* pIconTransformCom = dynamic_cast<CTransform*>(pIcon->Find_Component(TEXT("Com_Transform")));
		if (pIconTransformCom)
			pIconTransformCom->Start_BezierFlight(vStart, vControl, vEnd, fBezierSpeed);
	}
}



void CLobbyChest::Collided_With(CCollider* pOther, CCollider::COLLISION_STATE eCollisionState)
{
	/*if (CCollider::COLLISION_STATE::ENTER == eCollisionState &&
		TEXT("Player_Body") == pOther->Get_ColliderTag())
	{
		m_bSceneChanged = true;
	}*/
}

HRESULT CLobbyChest::Ready_Components()
{
	/* Com_Shader */
	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)))
		return E_FAIL;

	/* Com_Model */
	CModel::MODEL_DESC	pClosedModelDesc = {};
	pClosedModelDesc.bPickable = true;

	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_LobbyChest_Closed"),
		TEXT("Com_Model_ClosedChest"), reinterpret_cast<CComponent**>(&m_pClosedModelCom), &pClosedModelDesc))
		return E_FAIL;

	
	/* Com_Model */
	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_LobbyChest_Opened"),
		TEXT("Com_Model_OpenedChest"), reinterpret_cast<CComponent**>(&m_pOpenedModelCom)))
		return E_FAIL;


	/* Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC		SphereCollDesc{};

	SphereCollDesc.fRadius = 2.f;
	SphereCollDesc.vCenter = _float3(0.f, SphereCollDesc.fRadius, 0.f);
	SphereCollDesc.pGameObject = this;
	SphereCollDesc.CombinedWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	SphereCollDesc.pCollisionActivated = &m_bActivated;

	CComponent* pColliderSphereCom = Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereCollDesc);

	if (nullptr == pColliderSphereCom)
		return E_FAIL;

	m_pGameInstance->Add_ColliderCom(m_pGameInstance->Get_ChangedLevelIndex(), pColliderSphereCom, TEXT("LobbyChest"), TEXT("Level"));
	dynamic_cast<CCollider*>(pColliderSphereCom)->Set_ColliderRole(CCollider::ETC);
	dynamic_cast<CCollider*>(pColliderSphereCom)->Set_MouseCollider(true);

	return S_OK;
}

CLobbyChest* CLobbyChest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLobbyChest* pGameInstance = new CLobbyChest(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CLobbyChest");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject* CLobbyChest::Clone(void* pArg)
{
	CLobbyChest* pGameInstance = new CLobbyChest(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CLobbyChest");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CLobbyChest::Free()
{
	__super::Free();

	for (auto& pChestIcon : m_pChestIcons)
	{
		if (pChestIcon)
			Safe_Release(pChestIcon);
	}

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pClosedModelCom);
	Safe_Release(m_pOpenedModelCom);
	Safe_Release(m_pColliderCom);
}
