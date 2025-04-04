#pragma once

#include "Base.h"
#include <DirectXCollision.h>

BEGIN(Engine)
class CGameInstance;

class CPicking final : public CBase
{
private:
    CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    ~CPicking() override = default;

public:
    HRESULT Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);

    /* 마우스의 윈도우상의 위치를 구하고 */
    /* 뷰스페이스 상에서 마우스 레이로 변형하여 */
    /* 월드까지 변환시켜서 보관하자. */
    void    Compute_MouseRay();

private:
    ID3D11Device*           m_pDevice = { nullptr };
    ID3D11DeviceContext*    m_pContext = { nullptr };

private:
    CGameInstance*  m_pGameInstance = { nullptr };

    HWND        m_hWnd = {};
    _uint       m_iWinSizeX{}, m_iWinSizeY{};

    _float3     m_vMouseRay = {};
    _float3     m_vMousePos = {};

    _float3     m_vLocalMouseRay = {};
    _float3     m_vLocalMousePos = {};

public:
    static  CPicking*   Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
    void    Free()  override;
};

END
