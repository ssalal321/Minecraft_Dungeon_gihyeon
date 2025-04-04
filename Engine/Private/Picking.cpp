#include "Picking.h"
#include <DirectXCollision.h>
#include "GameInstance.h"

using namespace DirectX;

CPicking::CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CPicking::Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
    m_hWnd = hWnd;
    m_iWinSizeX = iWinSizeX;
    m_iWinSizeY = iWinSizeY;

	return S_OK;
}

void CPicking::Compute_MouseRay()
{
    POINT ptMouse = {};
    GetCursorPos(&ptMouse);
    ScreenToClient(m_hWnd, &ptMouse);

    // 1. NDC 좌표 변환
    _vector vPosition = XMVectorSet(
        (static_cast<_float>(ptMouse.x) / (static_cast<_float>(m_iWinSizeX) * 0.5f)) - 1.f,
        (static_cast<_float>(ptMouse.y) / (static_cast<_float>(m_iWinSizeY) * -0.5f)) + 1.f,
        0.0f, 1.0f
    );

    // 2. 투영 행렬 역변환
    _matrix  InvProjMatrix  = XMMatrixInverse(nullptr, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
    _vector  vTransformed   = XMVector3TransformCoord(vPosition, InvProjMatrix);

    // 3. 뷰 행렬 역변환
    _matrix  InvViewMatrix  = XMMatrixInverse(nullptr, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _vector  vMouseRay      = XMVector3TransformCoord(vTransformed, InvViewMatrix);
    _vector  vCamPosition   = XMLoadFloat4(m_pGameInstance->Get_CamPosition());

    // 레이 방향 벡터 정규화
    vMouseRay = XMVector3Normalize(vMouseRay - vCamPosition);

    XMStoreFloat3(&m_vMousePos, vCamPosition);
    XMStoreFloat3(&m_vMouseRay, vMouseRay);
}



CPicking* CPicking::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
    CPicking* pInstance = new CPicking(pDevice, pContext);

    if (FAILED(pInstance->Initialize(hWnd, iWinSizeX, iWinSizeY)))
    {
        MSG_BOX("Failed to Created : CPicking");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPicking::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
