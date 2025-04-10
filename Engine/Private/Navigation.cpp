#include "Navigation.h"
#include "Cell.h"

#include "Shader.h"
#include "GameInstance.h"

// static 변수 초기화 중
const _float4x4* CNavigation::m_pWorldMatrix = { nullptr };

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent ( pDevice, pContext )
{
}

CNavigation::CNavigation(const CNavigation& Prototype)
	: CComponent( Prototype ),
	  m_Cells ( Prototype.m_Cells )

#ifdef _DEBUG
	, m_pShader ( Prototype.m_pShader )
#endif
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);

#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif
}

HRESULT CNavigation::Initialize_Prototype(const _tchar* pNavigationDataFilePath)
{
	/*_ulong		dwByte = { };
	HANDLE		hFile = CreateFile(pNavigationDataFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	while (true)
	{
		_float3		vPoints[3] = {};

		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

		if (0 == dwByte)
			break;

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

	CloseHandle(hFile);*/

//
//	if (FAILED(SetUp_Neighbors()))
//		return E_FAIL;
//
#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNavigation::Initialize_Prototype()
{
#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	return S_OK;
}

void CNavigation::Update(const _float4x4* pWorldMatrix)
{
	m_pWorldMatrix = pWorldMatrix;

#ifdef _DEBUG
	if (m_pGameInstance->Key_Down('T') & 0x0001)
	{
		m_bLineRender = !m_bLineRender;

		for (auto& pCell : m_Cells)
			pCell->Set_RenderMode(m_bLineRender ? RENDER_MODE::LINE : RENDER_MODE::FILL);
	}
#endif
}
 
void CNavigation::Make_Cell(const _float3* fCellPoints)
{
	CCell* pCell = CCell::Create(m_pDevice, m_pContext, fCellPoints, m_Cells.size());
	if (nullptr == pCell)
		return;

	m_Cells.push_back(pCell);

	if (m_Cells.size() > 1)
		SetUp_Neighbors();
}

_bool CNavigation::Can_Move(_fvector vWorldPos)
{
	_matrix		WorldMatrixInv = XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pWorldMatrix));

	_vector		vPosition = XMVector3TransformCoord(vWorldPos, WorldMatrixInv);

	_int		iNeighborIndex = { -1 };

	if (true == m_Cells[m_iCurrentCellIndex]->Is_In(vPosition, &iNeighborIndex))
	{
		return true;
	}
	else
	{
		/*나간방향에 이웃이 있다면. */
		if (-1 != iNeighborIndex)
		{
			m_iCurrentCellIndex = iNeighborIndex;
			return true;
		}
		else
			return false;
	}
}

HRESULT CNavigation::SetUp_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
				pSourCell->SetUp_Neighbor(CCell::LINE_AB, pDestCell);

			else if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
				pSourCell->SetUp_Neighbor(CCell::LINE_BC, pDestCell);

			else if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
				pSourCell->SetUp_Neighbor(CCell::LINE_CA, pDestCell);
		}
	}

	return S_OK;
}

void CNavigation::SetUp_OnNavigation(CTransform* pTransform)
{
	_vector		vWorldPos = pTransform->Get_State(CTransform::STATE_POSITION);
	_matrix		WorldMatrixInv = XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pWorldMatrix));

	_vector		vPosition = XMVector3TransformCoord(vWorldPos, WorldMatrixInv);

	_float		fHeight = m_Cells[m_iCurrentCellIndex]->Compute_Height(vPosition);

	vPosition = XMVectorSetY(vPosition, fHeight);

	pTransform->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(vPosition, XMLoadFloat4x4(m_pWorldMatrix)));
}

#ifdef _DEBUG
HRESULT CNavigation::Render()
{
	_float4x4 WorldMatrix = *m_pWorldMatrix;
	WorldMatrix._42 += 0.1f; // 살짝 위로

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float4 vColor = _float4(1.f, 0.f, 0.f, 1.f); // 붉은색
	if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	m_pShader->Begin(0);

	for (auto& pCell : m_Cells)
		pCell->Render();

	return S_OK;
}
#endif


CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFilePath)
{
	CNavigation* pGameInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype(pNavigationDataFilePath)))
	{
		MSG_BOX("Failed to Create : CNavigation");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNavigation* pGameInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pGameInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CNavigation");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}


CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pGameInstance = new CNavigation(*this);

	if (FAILED(pGameInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CNavigation");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CNavigation::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();
}
