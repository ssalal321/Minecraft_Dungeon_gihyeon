#pragma once

#include "Base.h"
#include <DirectXCollision.h>

BEGIN(Engine)
class CGameInstance;

class CPicking final : public CBase
{
private:
    CPicking();
    ~CPicking() override = default;

public:
    HRESULT Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);

    /* 마우스의 윈도우상의 위치를 구하고 */
    /* 뷰스페이스 상에서 마우스 레이로 변형하여 */
    /* 월드까지 변환시켜서 보관하자. */
    
    _bool   Picked_Model(_float4& worldPickedPos, const _wstring& strGameObjectTag, _uint iLayerLevelIndex, const _wstring& strLayerTag);

	_bool   Picked_Vertex(_float3& fLocalPickedVertex, const _wstring& strGameObjectTag, _uint iLayerLevelIndex,
                          const _wstring& strLayerTag);
    void    Compute_MouseRay(_float4& worldMousePos, _float3& worldMouseRay);

private:
    CGameInstance*  m_pGameInstance = { nullptr };

    HWND        m_hWnd = {};
    _uint       m_iWinSizeX{}, m_iWinSizeY{};

    _float3     m_vMouseRay = {};
    _float3     m_vMousePos = {};

public:
    static  CPicking*   Create(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
    void    Free()  override;
};

END
