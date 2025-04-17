#include "LoungeMap.h"

#include <iostream>

#include "GameInstance.h"
#include "Player.h"

#ifdef _DEBUG
_uint CLoungeMap::m_iShaderPass = 0;
_bool CLoungeMap::m_bLineRender = false;
_bool CLoungeMap::m_bClickActive = false;
#endif

CLoungeMap::CLoungeMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject ( pDevice, pContext )
{
}

CLoungeMap::CLoungeMap(const CLoungeMap& Prototype)
	: CGameObject ( Prototype )
{
}

HRESULT CLoungeMap::Initialize_Prototype()
{
	/* 외부 데이터베이스를 통해서 값을 채운다. */

	return S_OK;
}

HRESULT CLoungeMap::Initialize(void* pArg)
{
	/* 원형의 데이터를 복제하여 사본을 만들고. */
	/* 추가적으로 필요한 데이터를 Arg로 받아와 실 사용하기위한 객체의 정보를 생성해준다. */	
	CGameObject::GAMEOBJECT_DESC		Desc{};

	Desc.pGameObjectTag = TEXT("GameObject_LoungeMap");
	Desc.fSpeedPerSec = 0.f;
	Desc.fRotationPerSec = 0.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(0.f, -54.95f, 0.f, 1.f));	// -54.95f

	m_pNavigationCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

	return S_OK;
}

void CLoungeMap::Priority_Update(_float fTimeDelta)
{
	
}

void CLoungeMap::Update(_float fTimeDelta)
{
#ifdef _DEBUG

	//if (m_pGameInstance->Key_Down(VK_F1))
	//	m_bClickActive = !m_bClickActive;

	//if (m_pGameInstance->Key_Down(VK_LBUTTON) && m_bClickActive)
	//{
	//	_float3		fLocalPickedVertex = {};

	//	if (m_pGameInstance->Picked_Vertex(fLocalPickedVertex, TEXT("Prototype_GameObject_LoungeMap"), LEVEL_GAMEPLAY, TEXT("Layer_BackGround")))
	//	{
	//		
	//		_vector vWorldPickedVertex = {};
	//		vWorldPickedVertex = XMVector3TransformCoord(XMLoadFloat3(&fLocalPickedVertex), XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()));
	//		_float3  fWorldPickedVertex;
	//		XMStoreFloat3(&fWorldPickedVertex, vWorldPickedVertex);

	//		std::cerr << "[피킹된 정점] X: " << fWorldPickedVertex.x
	//							 << " Y: " << fWorldPickedVertex.y
	//							 << " Z: " << fWorldPickedVertex.z << std::endl;
	//		

	//		m_fCellPoints[m_iPointNum] = fLocalPickedVertex;
	//		++m_iPointNum;

	//		// Navigation에 전달
	//		if (m_iPointNum == 3 && m_pNavigationCom)
	//		{
	//			m_pNavigationCom->Make_Cell(m_fCellPoints);
	//		}

	//		if (3 == m_iPointNum)
	//		{
	//			m_iPointNum = 0;
	//		}

	//		/*CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject(TEXT("Prototype_GameObject_PlayerHex"),
	//						LEVEL_GAMEPLAY, TEXT("Layer_Player")));
	//					pPlayer->Set_NextPosition({ fWorldPickedPos.x, fWorldPickedPos.y, fWorldPickedPos.z, 1.f });
	//					pPlayer->Change_State(PLAYER_STATE::WALK);*/
	//	}
	//}

	//if (m_pGameInstance->Key_Down(VK_RBUTTON) && m_bClickActive)
	//{
	//	m_pNavigationCom->Erase_Cell_Pick(m_pTransformCom->Get_WorldMatrix_Inverse());
	//}

	//if (m_pGameInstance->Key_Down(VK_BACK) && m_bClickActive)
	//{
	//	m_pNavigationCom->Erase_Cell_Last();
	//}
#endif
}
	

void CLoungeMap::Late_Update(_float fTimeDelta)
{

#ifdef _DEBUG
	if (m_pGameInstance->Key_Down('1') & 0x0001)
	{
		m_bLineRender = !m_bLineRender;

		m_iShaderPass = m_bLineRender ? 1 : 0;
	}

#endif

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);

}

HRESULT CLoungeMap::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShader_MeshCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;	

		if (FAILED(m_pShader_MeshCom->Begin(m_iShaderPass)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

#ifdef _DEBUG
	m_pNavigationCom->Render();
#endif

	return S_OK;
}

HRESULT CLoungeMap::Ready_Components()
{
	/* Com_Shader_Mesh */
	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader_Mesh"), reinterpret_cast<CComponent**>(&m_pShader_MeshCom)))
		return E_FAIL;

	/* Com_Model */
	if (nullptr == Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_LoungeMap"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom)))
		return E_FAIL;

	/* Com_Navigation */
	if (nullptr == __super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation_LoungeMap"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoungeMap::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShader_MeshCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShader_MeshCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader_MeshCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader_MeshCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	
	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShader_MeshCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShader_MeshCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShader_MeshCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CLoungeMap* CLoungeMap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLoungeMap* pGameInstance = new CLoungeMap(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CLoungeMap");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CGameObject* CLoungeMap::Clone(void* pArg)
{
	CLoungeMap* pGameInstance = new CLoungeMap(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CLoungeMap");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CLoungeMap::Free()
{
	__super::Free();

	Safe_Release(m_pShader_MeshCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pNavigationCom);
}
