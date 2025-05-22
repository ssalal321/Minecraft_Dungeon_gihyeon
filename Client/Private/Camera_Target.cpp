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

    // 타겟 위치
    _vector vTargetPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);

    // 고정 오프셋 적용 (월드 기준)
    _vector vDesiredPos = vTargetPos + XMVectorSet(m_vOffset.x, m_vOffset.y, m_vOffset.z, 0.f);

    // 현재 위치 보간 이동
    _vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _vector vNewPos = XMVectorLerp(vCurrentPos, vDesiredPos, m_fLagSpeed * fTimeDelta);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);

    // 머리보다 살짝 낮고, 앞쪽을 바라보게
    _vector vTargetLookPos = vTargetPos + XMVectorSet(0.f, 1.f, 1.f, 0.f);
    m_pTransformCom->LookAt_Full(vTargetLookPos);

    __super::Update_Camera();
}


void CCamera_Target::Late_Update(_float fTimeDelta)
{
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
