#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

// 플레이어를 따라다니는 타겟 카메라 클래스
class CCamera_Target final : public CCamera
{
public:
    // 카메라 설정용 구조체 (초기화 시 사용)
    typedef struct tagCameraTargetDesc : CAMERA_DESC {
        CTransform* pTargetTransform = nullptr;  // 추적할 대상의 트랜스폼
        _float3 vOffset = _float3(0.f, 5.f, -10.f);     // 타겟 기준 오프셋 (위/뒤)
        _float fLagSpeed = 5.f;                         // 위치 보간 속도 (높을수록 빠르게 따라감)
    } CAMERA_TARGET_DESC;

private:
    CCamera_Target(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CCamera_Target(const CCamera_Target& rhs);
    ~CCamera_Target() override = default;

public:
    HRESULT		Initialize_Prototype() override;
    HRESULT		Initialize(void* pArg) override;
    void		Priority_Update(_float fTimeDelta) override;
    void		Update(_float fTimeDelta) override;
    void		Late_Update(_float fTimeDelta) override;
    HRESULT		Render() override;

private:
    CTransform* m_pTargetTransform = nullptr;   // 추적 대상 (플레이어 등)
    _float3     m_vOffset = {};                                // 대상 기준의 상대적 위치
    _float m_fLagSpeed = 5.f;                         // 보간 속도


public:
    static  CCamera_Target* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CGameObject*    Clone(void* pArg) override;
    void    Free() override;
};

END