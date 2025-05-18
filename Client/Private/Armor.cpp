#include "Armor.h"
#include "GameInstance.h"
#include "Item.h"

CArmor::CArmor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CItem(pDevice, pContext)
{

}

CArmor::CArmor(const CArmor& Prototype)
	: CItem(Prototype)
{

}

HRESULT CArmor::Initialize_Prototype()
{
	/* 외부 데이터베이스를 통해서 값을 채운다. */

	return S_OK;
}

HRESULT CArmor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	/* 원형의 데이터를 복제하여 사본을 만들고. */
	/* 추가적으로 필요한 데이터를 Arg로 받아와 실 사용하기위한 객체의 정보를 생성해준다. */	

	if (FAILED(Ready_Components()))
		return E_FAIL;


	// ShortBow 세팅
	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(90.f));
	return S_OK;
}

void CArmor::Priority_Update(_float fTimeDelta)
{
	if (false == m_bItemActive)
		return;
}

void CArmor::Update(_float fTimeDelta)
{
	if (false == m_bItemActive)
		return;
}

void CArmor::Late_Update(_float fTimeDelta)
{
	if (false == m_bItemActive)
		return; 

	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)	
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);	

	XMStoreFloat4x4(&m_CombinedWorldMatrix, 
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * 
		SocketMatrix *
		XMLoadFloat4x4(m_pParentWorldMatrix));

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CArmor::Render()
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

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(static_cast<_uint>(i))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CArmor::Ready_Components()
{
	/* Com_Shader */
	if (nullptr == Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}


CPartObject* CArmor::Find_PartObject(const _wstring& strPartObjectTag)
{
	auto	iter = m_ArmorPartObjects.find(strPartObjectTag);

	if (iter == m_ArmorPartObjects.end())
		return nullptr;

	return iter->second;
}

HRESULT CArmor::Add_PartObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag,
                               const _wstring& strPartObjectTag, void* pArg)
{
	if (nullptr != Find_PartObject(strPartObjectTag))
		return E_FAIL;

	CPartObject* pPartObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTOTYPE_GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, pArg));
	if (nullptr == pPartObject)
		return E_FAIL;

	m_ArmorPartObjects.emplace(strPartObjectTag, pPartObject);

	return S_OK;
}

void CArmor::Free()
{
	__super::Free();

	for (auto& Pair : m_ArmorPartObjects)
		Safe_Release(Pair.second);

	m_ArmorPartObjects.clear();
}
