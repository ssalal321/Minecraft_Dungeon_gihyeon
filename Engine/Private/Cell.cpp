#include "Cell.h"
#include "VIBuffer_Cell.h"

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pContext { pContext }
{
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pDevice);
}

void CCell::Set_RenderMode(RENDER_MODE eMode)
{
#ifdef _DEBUG
    if (m_pVIBuffer)
        m_pVIBuffer->Set_RenderMode(eMode);
#endif
}

HRESULT CCell::Initialize(const _float3* pPoints, _int iIndex)
{
    memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);

    m_iIndex = iIndex;

    _float3 vLines[LINE_END] = {};

    XMStoreFloat3(&vLines[LINE_AB], XMVector3Normalize(XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_A])));
    XMStoreFloat3(&vLines[LINE_BC], XMVector3Normalize(XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(&m_vPoints[POINT_B])));
    XMStoreFloat3(&vLines[LINE_CA], XMVector3Normalize(XMLoadFloat3(&m_vPoints[POINT_A]) - XMLoadFloat3(&m_vPoints[POINT_C])));

    for (size_t i = 0; i < LINE_END; i++)
        m_vNormals[i] = _float3(-vLines[i].z, 0.f, vLines[i].x);

    XMStoreFloat4(&m_vPlane,
        XMPlaneFromPoints(
            XMVectorSetW(XMLoadFloat3(&m_vPoints[POINT_A]), 1.f),
            XMVectorSetW(XMLoadFloat3(&m_vPoints[POINT_B]), 1.f),
            XMVectorSetW(XMLoadFloat3(&m_vPoints[POINT_C]), 1.f)));

#ifdef _DEBUG
    m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
    if (nullptr == m_pVIBuffer)
        return E_FAIL;
#endif

    return S_OK;
}

HRESULT CCell::Render()
{
    if (FAILED(m_pVIBuffer->Input_Assembler()))
        return E_FAIL;

    return m_pVIBuffer->Render();    
}

_bool CCell::Is_In(_fvector vPosition, _int* pNeighborIndex)
{
    for (size_t i = 0; i < LINE_END; i++)
    {
        _vector vDir = vPosition - XMLoadFloat3(&m_vPoints[i]);

        if (0 < XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir), XMLoadFloat3(&m_vNormals[i]))))
        {
            *pNeighborIndex = m_iNeighborCellIndices[i];
            return false;
        }    
    }
    return true;
}

_bool CCell::Compare_Points(_fvector vSourPoint, _fvector vDestPoint)
{
    
    /*XMVectorEqual();*/
    if (true == XMVector3Equal(vSourPoint, XMLoadFloat3(&m_vPoints[POINT_A])))
    {
        if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_vPoints[POINT_B])))
            return true;
        if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_vPoints[POINT_C])))
            return true;
    }

    if (true == XMVector3Equal(vSourPoint, XMLoadFloat3(&m_vPoints[POINT_B])))
    {
        if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_vPoints[POINT_C])))
            return true;
        if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_vPoints[POINT_A])))
            return true;
    }

    if (true == XMVector3Equal(vSourPoint, XMLoadFloat3(&m_vPoints[POINT_C])))
    {
        if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_vPoints[POINT_A])))
            return true;
        if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_vPoints[POINT_B])))
            return true;
    }


    return _bool();
}

_float CCell::Compute_Height(_fvector vPosition)
{
    /* ax + by + cz + d = 0 */
    /* y = (-ax - cz - d) / b */

    return (-m_vPlane.x * XMVectorGetX(vPosition) - m_vPlane.z * XMVectorGetZ(vPosition) - m_vPlane.w) / m_vPlane.y;
}

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex)
{
    CCell* pGameInstance = new CCell(pDevice, pContext);

    if (FAILED(pGameInstance->Initialize(pPoints, iIndex)))
    {
        MSG_BOX("Failed to Create : CCell");
        Safe_Release(pGameInstance);
    }

    return pGameInstance;
}

void CCell::Free()
{
    __super::Free();

#ifdef _DEBUG
    Safe_Release(m_pVIBuffer);
#endif

    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}
