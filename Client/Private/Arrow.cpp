#include "Arrow.h"

_int  CArrow::m_iArrowID = 0;

CArrow::CArrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CArrow::CArrow(const CArrow& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CArrow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CArrow::Initialize(void* pArg)
{
	const _wstring& arrowGameObjectTag = TEXT("GameObject_Arrow") + to_wstring(m_iArrowID++);

	m_pArrowDesc = new GAMEOBJECT_DESC(arrowGameObjectTag, 0.f, 3.f);

	if (FAILED(__super::Initialize(m_pArrowDesc)))
		return E_FAIL;


	return S_OK;
}

void CArrow::Priority_Update(_float fTimeDelta)
{

}

void CArrow::Update(_float fTimeDelta)
{
	
}

void CArrow::Late_Update(_float fTimeDelta)
{
	
}

HRESULT CArrow::Render()
{
	return S_OK;
}

void CArrow::Shoot(_float3 startPos, _float3 lookPos, _float fTimeDelta)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&startPos));
	m_pTransformCom->LookAt(XMLoadFloat3(&lookPos));
	m_pTransformCom->Go_Straight(fTimeDelta);

	m_bActive = true;
}

void CArrow::Reset()
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 0.f, 0.f, 0.f});
	m_pTransformCom->LookAt({ 0.f, 0.f, 0.f });

	m_bActive = false;
}

CArrow* CArrow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CArrow* pGameInstance = new CArrow(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CArrow");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CGameObject* CArrow::Clone(void* pArg)
{
	CArrow* pGameInstance = new CArrow(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CArrow");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


void CArrow::Free()
{
	__super::Free();

	Safe_Delete(m_pArrowDesc);
}
