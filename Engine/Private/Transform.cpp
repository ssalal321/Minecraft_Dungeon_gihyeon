#include "Transform.h"

#include <iostream>

#include "Navigation.h"
#include "Shader.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
{

}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pArg);
	if (nullptr == pDesc)
		return E_FAIL;

	m_fSpeedPerSec = pDesc->fSpeedPerSec;
	m_fRotationPerSec = pDesc->fRotationPerSec;	

	return S_OK;
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
	
}

_float3 CTransform::Compute_Scaled()
{
	return _float3(
		XMVectorGetX(XMVector3Length(Get_State(STATE_RIGHT))), 
		XMVectorGetX(XMVector3Length(Get_State(STATE_UP))),
		XMVectorGetX(XMVector3Length(Get_State(STATE_LOOK)))
	);
}

void CTransform::SetUp_Scale(_float fScaleX, _float fScaleY, _float fScaleZ)
{
	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * fScaleX);
	Set_State(STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * fScaleY);
	Set_State(STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * fScaleZ);
}

void CTransform::Go_Straight(_float fTimeDelta, CNavigation* pNavigation, _float fSpeedFactor)
{
	_vector		vLook = Get_State(STATE::STATE_LOOK);
	/*vLook = XMVectorSetY(vLook, 0.f);
	vLook = XMVector3Normalize(vLook);*/
	_vector		vPosition = Get_State(STATE::STATE_POSITION);

	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta * fSpeedFactor;

	if (nullptr == pNavigation || true == pNavigation->Can_Move(vPosition))
		Set_State(STATE_POSITION, vPosition);
}


void CTransform::Go_Left(_float fTimeDelta)
{
	_vector		vRight = Get_State(STATE::STATE_RIGHT);
	_vector		vPosition = Get_State(STATE::STATE_POSITION);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}


void CTransform::Go_Right(_float fTimeDelta)
{
	_vector		vRight = Get_State(STATE::STATE_RIGHT);
	_vector		vPosition = Get_State(STATE::STATE_POSITION);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_vector		vLook = Get_State(STATE::STATE_LOOK);
	_vector		vPosition = Get_State(STATE::STATE_POSITION);

	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Jump_Start(_float fJumpVelocity)
{
	if (!m_bIsJumping)
	{
		m_bIsJumping = true;
		m_fJumpVelocity = fJumpVelocity;

		// 슬라임의 현재 y 위치를 기준으로 점프 시작 위치 초기화
		_vector vPos = Get_State(STATE_POSITION);
		m_fCurrentY = XMVectorGetY(vPos);

		std::cerr << "[점프]" << endl;
	}
}

void CTransform::Jump(_float fTimeDelta, CNavigation* pNavigation)
{
    if (!m_bIsJumping)
        return;

    // 현재 위치 정보
    _vector vPosition = Get_State(STATE_POSITION);  // 슬라임 현재 위치 정보
    _float3 position = {};
    XMStoreFloat3(&position, vPosition);

    // 중력 반영: 점프 높이 증가 → 속도 감소
    m_fCurrentY		+= m_fJumpVelocity * fTimeDelta;
    m_fJumpVelocity += m_fGravity * fTimeDelta;

    position.y = m_fCurrentY;
    vPosition = XMLoadFloat3(&position);

	std::cerr << "[높이 :" << position.y << "]" << std::endl;

    // 네비게이션 상태 복원 여부 확인
    if (nullptr != pNavigation)
    {
    	if (pNavigation->Check_If_Grounded(this))
    	{
            // 착지 처리
            pNavigation->SetUp_On_Navigation(this);
            m_bIsJumping = false;

            std::cerr << "[착지]" << std::endl;
            return;
        }
    }

    // 점프 중 위치 갱신
    Set_State(STATE_POSITION, vPosition);
}



void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	/*XMVector4Transform();
	XMVector3TransformCoord();*/
	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	// 기존 방향 벡터를 가져옴
	_vector vRight	= Get_State(STATE_RIGHT);
	_vector vUp		= Get_State(STATE_UP);
	_vector vLook	= Get_State(STATE_LOOK);

	// 입력된 축을 중심으로 회전 행렬 생성
	_matrix RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	// 기존 방향 벡터를 회전 행렬로 변환하여 적용 (누적 회전)
	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}


void CTransform::LookAt(_fvector vAt)
{
	_float3		vScaled = Compute_Scaled();

	_vector		vPosition = Get_State(STATE_POSITION);

	_vector		vLook = vAt - vPosition;
	vLook = XMVectorSetY(vLook, 0.f);	// 직진만 할 수 있도록

	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	_vector		vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransform* pGameInstance = new CTransform(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CTransform");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CComponent* CTransform::Clone(void* pArg)
{
	CTransform* pGameInstance = new CTransform(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CTransform");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


void CTransform::Free()
{
	__super::Free();
}
