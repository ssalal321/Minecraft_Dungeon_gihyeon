#include "Picking.h"
#include <DirectXCollision.h>
#include "GameInstance.h"
#include "GameObject.h"

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

_bool CPicking::Picked_Model(_float3& fLocalPickedPos, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag)
{
    _float3  fWorldMousePos, fWorldMouseRay = {};
   
    Compute_MouseRay(fWorldMousePos, fWorldMouseRay);

    CGameObject*  pPickedObject = m_pGameInstance->Find_GameObject(strPrototypeTag, iLayerLevelIndex, strLayerTag);
    CModel*       pPickedObjModelCom = dynamic_cast<CModel*>(pPickedObject->Find_Component(TEXT("Com_Model")));
    CTransform*   pPickedObjTransformCom = dynamic_cast<CTransform*>(pPickedObject->Find_Component(TEXT("Com_Transform")));
    const _float4x4&  pPickedObjWorldMatrix = pPickedObjTransformCom->Get_WorldMatrix();

    // 2. LoungeMap에 피킹 요청 (BoundingBox 충돌 체크)
    if (pPickedObjModelCom->Picking_Model(fWorldMousePos, fWorldMouseRay, fLocalPickedPos, pPickedObjWorldMatrix))
    {
        return true;
    }

    return false;
}

_bool CPicking::Picked_Vertex(_float3& fLocalPickedVertex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag)
{
    _float3  fWorldMousePos, fWorldMouseRay = {};

    Compute_MouseRay(fWorldMousePos, fWorldMouseRay);

    CGameObject* pPickedObject = m_pGameInstance->Find_GameObject(strPrototypeTag, iLayerLevelIndex, strLayerTag);
    CModel* pPickedObjModelCom = dynamic_cast<CModel*>(pPickedObject->Find_Component(TEXT("Com_Model")));
    CTransform* pPickedObjTransformCom = dynamic_cast<CTransform*>(pPickedObject->Find_Component(TEXT("Com_Transform")));
    const _float4x4& pPickedObjWorldMatrix = pPickedObjTransformCom->Get_WorldMatrix();

    // 2. LoungeMap에 피킹 요청 (BoundingBox 충돌 체크)
    if (pPickedObjModelCom->Picking_Vertex(fWorldMousePos, fWorldMouseRay, fLocalPickedVertex, pPickedObjWorldMatrix))
    {
        return true;
    }

    return false;
}

void CPicking::Compute_MouseRay(_float3& worldMousePos, _float3& worldMouseRay)
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
    _matrix  InvProjMatrix  = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_PROJ);
    _vector  vViewPosition  = XMVector3TransformCoord(vPosition, InvProjMatrix);

    // 3. 뷰 행렬 역변환
    _matrix  InvViewMatrix  = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW);
    _vector  vWorldPosition = XMVector3TransformCoord(vViewPosition, InvViewMatrix);
    _vector  vCamPosition   = XMLoadFloat4(m_pGameInstance->Get_CamPosition());

    // 마우스 포지션과 정규화된 레이 반환
    XMStoreFloat3(&worldMousePos, vCamPosition);
	XMStoreFloat3(&worldMouseRay, XMVector3Normalize(vWorldPosition - vCamPosition));
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