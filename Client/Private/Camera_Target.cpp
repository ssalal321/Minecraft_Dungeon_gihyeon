#include "Camera_Target.h"
#include "Transform.h"
#include "GameInstance.h"

// 생성자
CCamera_Target::CCamera_Target(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera(pDevice, pContext)
{
}

CCamera_Target::CCamera_Target(const CCamera_Target& rhs)
    : CCamera(rhs)
{
}

// 프로토타입 초기화 (사용 안 해도 기본 템플릿 구조상 구현됨)
HRESULT CCamera_Target::Initialize_Prototype()
{
    return S_OK;
}

// 실제 인스턴스 초기화
HRESULT CCamera_Target::Initialize(void* pArg)
{
    if (!pArg)
        return E_FAIL;

    CAMERA_TARGET_DESC* pDesc = static_cast<CAMERA_TARGET_DESC*>(pArg);
    if (!pDesc || !pDesc->pTargetTransform)
        return E_FAIL;

    _float3 eye = {}, at = {};
    XMStoreFloat3(&eye, pDesc->pTargetTransform->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&pDesc->vOffset));
    XMStoreFloat3(&at, pDesc->pTargetTransform->Get_State(CTransform::STATE_POSITION));
    CAMERA_DESC camDesc = {};

    camDesc.strGameObjectTag = pDesc->strGameObjectTag;
    camDesc.vEye = eye;
    camDesc.vAt = at;
    camDesc.fFov = pDesc->fFov;
    camDesc.fNear = pDesc->fNear;
    camDesc.fFar = pDesc->fFar;

    if (FAILED(__super::Initialize(&camDesc)))
        return E_FAIL;

    // 본인 고유 파라미터 저장
    m_pTargetTransform = pDesc->pTargetTransform;
    m_vOffset = pDesc->vOffset;
    m_fLagSpeed = pDesc->fLagSpeed;

    return S_OK;
}


void CCamera_Target::Priority_Update(_float fTimeDelta)
{
}


// 매 프레임 위치 및 시점 업데이트
void CCamera_Target::Update(_float fTimeDelta)
{
    if (!m_pTargetTransform)
        return;

    _vector vTargetPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);

    // 초기 Y 고정
    if (!m_bInitFixedY)
    {
        m_fFixedTargetY = XMVectorGetY(vTargetPos);
        m_bInitFixedY = true;
    }

    // 초기 XZ 고정
    _float fTargetX = XMVectorGetX(vTargetPos);
    _float fTargetZ = XMVectorGetZ(vTargetPos);

    if (!m_bInitXZ)
    {
        m_vSmoothedTargetXZ = { fTargetX, fTargetZ };
        m_bInitXZ = true;
    }

    // XZ 감쇠 (지수 보간)
    _float smoothingXZ = 1.f - pow(0.005f, fTimeDelta);  // 낮을수록 더 부드러움
    m_vSmoothedTargetXZ.x = Lerp(m_vSmoothedTargetXZ.x, fTargetX, smoothingXZ);
    m_vSmoothedTargetXZ.y = Lerp(m_vSmoothedTargetXZ.y, fTargetZ, smoothingXZ);

    // 최종 카메라 위치
    _vector vCameraPos = XMVectorSet(
        m_vSmoothedTargetXZ.x + m_vOffset.x,
        m_fFixedTargetY + m_vOffset.y,
        m_vSmoothedTargetXZ.y + m_vOffset.z,
        1.f
    );
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCameraPos);

    // 시선도 감쇠된 위치 기준
    _vector vLookTarget = XMVectorSet(
        m_vSmoothedTargetXZ.x,
        m_fFixedTargetY + 1.f,   // 플레이어보다 더 아래 지점을 바라보게
        m_vSmoothedTargetXZ.y,
        1.f
    );
    m_pTransformCom->LookAt_Full(vLookTarget);

    __super::Update_Camera();
}


void CCamera_Target::Late_Update(_float fTimeDelta)
{
}


_vector CCamera_Target::SmoothFollow(_vector current, _vector target, _float smoothTime, _float deltaTime)
{
    const float omega = 2.0f / smoothTime;
    const float x = omega * deltaTime;
    const float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

    _vector change = current - target;
    _vector temp = (m_vCameraVelocity + change * omega) * deltaTime;
    m_vCameraVelocity = (m_vCameraVelocity - temp * omega) * exp;

    return target + (change + temp) * exp;
}


CGameObject* CCamera_Target::Clone(void* pArg)
{
    CCamera_Target* pClone = new CCamera_Target(*this);
    if (FAILED(pClone->Initialize(pArg)))
    {
        MSG_BOX("Clone CCamera_Target Failed");
        Safe_Release(pClone);
    }
    return pClone;
}


HRESULT CCamera_Target::Render()
{
    return S_OK;
}


CCamera_Target* CCamera_Target::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Target* pInstance = new CCamera_Target(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create CCamera_Target Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}


void CCamera_Target::Free()
{
    __super::Free();
}
