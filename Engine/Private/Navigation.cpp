#include "Navigation.h"

#include <iomanip>
#include <iostream>
#include <sstream>

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

std::string CNavigation::Make_Cell_Key(const _float3* fCellPoints) const
{
	std::vector<_float3> verts = { fCellPoints[0], fCellPoints[1], fCellPoints[2] };

	// 정렬해서 일관된 키 보장
	std::sort(verts.begin(), verts.end(), 
		[](const _float3& a, const _float3& b) 
		{
			if (a.x != b.x) return a.x < b.x;
			if (a.y != b.y) return a.y < b.y;

			return a.z < b.z;
		}
	);

	/* 문자열을 스트림 형식으로 조립할 수 있는 출력용 문자열 스트림 객체 */
	std::ostringstream oss;

	/* std::fixed : 고정 소수점(fixed-point) 형식으로 출력
	   std::setprecision(3) : 숫자의 소수점 이하 자릿수 개수를 3자리로 설정 (단, std::fixed와 같이 써야 소수점 자릿수로 인식) */
	oss << std::fixed << std::setprecision(3);

	for (int i = 0; i < 3; ++i)
	{
		oss << ROUND_FLOAT(verts[i].x) << "_"
			<< ROUND_FLOAT(verts[i].y) << "_"
			<< ROUND_FLOAT(verts[i].z);
		if (i < 2) oss << "_";  // 마지막 빼고는 Cell 넘어갈 때마다 _ 넣기
	}

	return oss.str();
}

void CNavigation::Make_Cell(const _float3* fCellPoints, const _float4x4* WorldMatrix)
{
	// 키 생성
	std::string triangleKey = Make_Cell_Key(fCellPoints);

	// 이미 존재하는 삼각형이면 셀 생성 생략
	if (m_TriangleSet.find(triangleKey) != m_TriangleSet.end())
		return;

	// 중복이 아니라면 키 추가
	m_TriangleSet.insert(triangleKey);

	// 셀 생성
	CCell* pCell = CCell::Create(m_pDevice, m_pContext, fCellPoints, m_Cells.size(), WorldMatrix);
	if (nullptr == pCell)
		return;

	m_Cells.push_back(pCell);

	std::cerr << "[셀 개수] : " << m_Cells.size() << std::endl;

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

void CNavigation::SetUp_On_Navigation(CTransform* pTransform)
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
	WorldMatrix._42 += 0.05f; // 살짝 위로

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
