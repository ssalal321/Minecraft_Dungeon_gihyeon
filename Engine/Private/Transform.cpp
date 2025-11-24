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
	_vector  vLook			 = Get_State(STATE_LOOK);
	_vector  vPrevPosition	 = Get_State(STATE_POSITION);
	_vector  vMovingPosition = vPrevPosition + XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta * fSpeedFactor;

	_vector  vSlidingPosition = vPrevPosition;

	if (nullptr != pNavigation)
	{
		if (pNavigation->Can_Move(vMovingPosition))
		{
			Set_State(STATE_POSITION, vMovingPosition);
		}
		else if (pNavigation->Can_Slide(vPrevPosition, vMovingPosition, vSlidingPosition))
		{
			Set_State(STATE_POSITION, vSlidingPosition);
		}
	}
	else
	{
		Set_State(STATE_POSITION, vMovingPosition);
	}
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

		// 현재 y 위치를 기준으로 점프 시작 위치 초기화
		_vector vPos = Get_State(STATE_POSITION);
		m_fCurrentY = XMVectorGetY(vPos);
	}
}

void CTransform::Jump(_float fTimeDelta, CNavigation* pNavigation)
{
    if (!m_bIsJumping)
        return;

    // 현재 위치 정보
    _vector		vPosition = Get_State(STATE_POSITION);  // 현재 위치 정보
    _float4		position = {};
    XMStoreFloat4(&position, vPosition);

    // 중력 반영: 점프 높이 증가 → 속도 감소
    m_fCurrentY		+= m_fJumpVelocity * fTimeDelta;

    m_fJumpVelocity += m_fGravity * fTimeDelta;

    position.y = m_fCurrentY;
    vPosition = XMLoadFloat4(&position);

	Set_State(STATE_POSITION, vPosition);

    // 네비게이션 상태 복원 여부 확인
    if (nullptr != pNavigation)
    {
    	if (pNavigation->Check_If_Grounded(this))
    	{
            // 착지 처리
            pNavigation->SetUp_On_Navigation(this);
            m_bIsJumping = false;

    		return;
        }
    }

    // 점프 중 위치 갱신
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp    = Get_State(STATE_UP);
	_vector		vLook  = Get_State(STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	/*XMVector4Transform();
	XMVector3TransformCoord();*/
	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Turn_Around_Offset(_fvector vAxis, _float fRadian, _float fOffsetDistance)
{
	// 1. 현재 상태 가져오기
	_vector vRight = Get_State(STATE_RIGHT);
	_vector vUp = Get_State(STATE_UP);
	_vector vLook = Get_State(STATE_LOOK);
	_vector vPosition = Get_State(STATE_POSITION);

	// 2. 오프셋 계산 (바라보는 방향 기준 앞쪽)
	_vector vOffset = XMVector3Normalize(vLook) * fOffsetDistance;

	// 3. 위치를 임시로 앞쪽으로 이동
	_vector vTempPos = vPosition + vOffset;

	// 4. 회전 행렬 생성
	_matrix RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	// 5. 방향 벡터 회전
	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	// 6. 위치 회전 (오프셋 위치 기준 회전된 위치로 계산)
	vTempPos = XMVector3TransformCoord(vTempPos, RotationMatrix);

	// 7. 최종 위치 = 회전된 오프셋 위치 - 오프셋
	vPosition = vTempPos - XMVector3Normalize(vLook) * fOffsetDistance;

	// 8. 저장
	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
	Set_State(STATE_POSITION, vPosition);
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

void CTransform::LookAt_Full(_fvector vAt)
{
	_float3		vScaled = Compute_Scaled();

	_vector		vPosition = Get_State(STATE_POSITION);

	_vector		vLook = vAt - vPosition;
	
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	_vector		vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}


void CTransform::Add_Momentum(_vector vImpulse)
{
	_vector vOld = XMLoadFloat3(&m_Velocity);
	vOld += vImpulse;
	XMStoreFloat3(&m_Velocity, vOld);

	std::cerr << "velocity x: " << m_Velocity.x << ", " << "velocity y: " << m_Velocity.y << ", " << "velocity z: " << m_Velocity.z << endl;
}

void CTransform::Update_Momentum(_float fTimeDelta)
{
	_vector vVel = XMLoadFloat3(&m_Velocity);
	if (XMVector3Equal(vVel, XMVectorZero()))
		return;

	_vector vCurPos = Get_State(STATE_POSITION);
	_vector vNextPos = vCurPos + vVel * fTimeDelta;
	_vector vSlidePos = vCurPos;

	if (m_pNavigationCom)
	{
		if (m_pNavigationCom->Can_Move(vNextPos))
		{
			Set_State(STATE_POSITION, vNextPos);
		}
		else if (m_pNavigationCom->Can_Slide(vCurPos, vNextPos, vSlidePos))
		{
			Set_State(STATE_POSITION, vSlidePos);
		}
		else
		{
			// 아무것도 안 됐으면 멈춘다.
			vVel = XMVectorZero();
			XMStoreFloat3(&m_Velocity, vVel);
			return;
		}
	}
	else
	{
		Set_State(STATE_POSITION, vNextPos);
	}

	// 감쇠
	vVel *= 0.85f;
	if (XMVectorGetX(XMVector3Length(vVel)) < 0.001f)
		vVel = XMVectorZero();

	XMStoreFloat3(&m_Velocity, vVel);
}


void CTransform::Start_BezierFlight(const XMFLOAT3& vStart, const XMFLOAT3& vControl, const XMFLOAT3& vEnd, _float fSpeed)
{
	m_vBezierStart		= vStart;
	m_vBezierControl	= vControl;
	m_vBezierEnd		= vEnd;

	m_fBezierT		= 0.f;
	m_fBezierSpeed	= fSpeed;
	m_bBezierFlying = true;
}

void CTransform::Update_BezierFlight(_float fDeltaTime)
{
	if (!m_bBezierFlying)
		return;

	m_fBezierT += fDeltaTime * m_fBezierSpeed;

	if (m_fBezierT >= 1.f)
	{
		m_fBezierT = 1.f;
		m_bBezierFlying = false;
	}

	// 베지어 계산 (2차)
	_float  t	= m_fBezierT;
	_float  u	= 1.f - t;
	_float  tt	= t * t;
	_float  uu	= u * u;

	_float4   result = {};
	result.x = uu * m_vBezierStart.x + 2 * u * t * m_vBezierControl.x + tt * m_vBezierEnd.x;
	result.y = uu * m_vBezierStart.y + 2 * u * t * m_vBezierControl.y + tt * m_vBezierEnd.y;
	result.z = uu * m_vBezierStart.z + 2 * u * t * m_vBezierControl.z + tt * m_vBezierEnd.z;
	result.w = 1.f;

	Set_State(STATE_POSITION, XMLoadFloat4(&result));
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
