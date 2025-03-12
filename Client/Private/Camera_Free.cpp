#include "Camera_Free.h"
#include "GameInstance.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera { pDevice, pContext }
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& Prototype)
	: CCamera{ Prototype }
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Free::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	CAMERA_FREE_DESC* pDesc = static_cast<CAMERA_FREE_DESC*>(pArg);
	m_fMouseSensor = pDesc->fMouseSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Free::Priority_Update(_float fTimeDelta)
{
}

void CCamera_Free::Update(_float fTimeDelta)
{
	if (GetKeyState('W') & 0x8000)
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	if (GetKeyState('S') & 0x8000)
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}
	if (GetKeyState('A') & 0x8000)
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}
	if (GetKeyState('D') & 0x8000)
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}

	_long		MouseMove = {};
	
	if (MouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::DIMS_X))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensor);
		
	}

	if (MouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::DIMS_Y))
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * m_fMouseSensor);
	}

	__super::Update_Camera();

}

void CCamera_Free::Last_Update(_float fTimeDelta)
{
}

HRESULT CCamera_Free::Render()
{
	return S_OK;
}

CCamera_Free* CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Free* pGameInstance = new CCamera_Free(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CCamera_Free");
		Safe_Release(pGameInstance);
	}
	return pGameInstance;
}


CGameObject* CCamera_Free::Clone(void* pArg)
{
	CCamera_Free* pGameInstance = new CCamera_Free(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CCamera_Free");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CCamera_Free::Free()
{
	__super::Free();

}
