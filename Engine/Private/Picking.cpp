#include "Picking.h"
#include <DirectXCollision.h>
#include "GameInstance.h"
#include "GameObject.h"

using namespace DirectX;

CPicking::CPicking()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CPicking::Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
    m_hWnd = hWnd;
    m_iWinSizeX = iWinSizeX;
    m_iWinSizeY = iWinSizeY;

	return S_OK;
}

_bool CPicking::Picked_Model(_float4& worldPickedPos, const _wstring& strGameObjectTag, _uint iLayerLevelIndex, const _wstring& strLayerTag)
{
    _float4  fWorldMousePos = {};
    _float3  fWorldMouseRay = {};
    _float3  fLocalPickedPos = {};

    Compute_MouseRay(fWorldMousePos, fWorldMouseRay);

    CGameObject*  pPickedObject = m_pGameInstance->Find_GameObject(strGameObjectTag, iLayerLevelIndex, strLayerTag);
    CModel*       pPickedObjModelCom = dynamic_cast<CModel*>(pPickedObject->Find_Component(TEXT("Com_Model")));
    CTransform*   pPickedObjTransformCom = dynamic_cast<CTransform*>(pPickedObject->Find_Component(TEXT("Com_Transform")));
    const _float4x4&  pPickedObjWorldMatrix = pPickedObjTransformCom->Get_WorldMatrix();

    // 2. LoungeMap에 피킹 요청 (BoundingBox 충돌 체크)
    if (pPickedObjModelCom->Picking_Model(fWorldMousePos, fWorldMouseRay, fLocalPickedPos, pPickedObjWorldMatrix))
    {
        _float4  localPickedPos = { fLocalPickedPos.x, fLocalPickedPos.y, fLocalPickedPos.z, 1.f };
        _vector  vWorldPickedPos = XMVector4Transform(XMLoadFloat4(&localPickedPos), XMLoadFloat4x4(&pPickedObjWorldMatrix));
        XMStoreFloat4(&worldPickedPos, vWorldPickedPos);

        return true;
    }

    return false;
}

_bool CPicking::Picked_Vertex(_float3& fLocalPickedVertex, const _wstring& strGameObjectTag, _uint iLayerLevelIndex, const _wstring& strLayerTag)
{
    _float4  fWorldMousePos = {};
	_float3  fWorldMouseRay = {};

    Compute_MouseRay(fWorldMousePos, fWorldMouseRay);

    CGameObject* pPickedObject = m_pGameInstance->Find_GameObject(strGameObjectTag, iLayerLevelIndex, strLayerTag);
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

void CPicking::Compute_MouseRay(_float4& worldMousePos, _float3& worldMouseRay)
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
    XMStoreFloat4(&worldMousePos, vCamPosition);
	XMStoreFloat3(&worldMouseRay, XMVector3Normalize(vWorldPosition - vCamPosition));
}


CPicking* CPicking::Create(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
    CPicking* pInstance = new CPicking();

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

    Safe_Release(m_pGameInstance);
}