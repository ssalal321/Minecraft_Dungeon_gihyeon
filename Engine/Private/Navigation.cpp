#include "Navigation.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#include "Cell.h"

#include "Shader.h"
#include "GameInstance.h"

// static 변수 초기화 중
const _float4x4* CNavigation::m_pWorldMatrix = { nullptr };
#ifdef _DEBUG
_uint CNavigation::m_iShaderPass = 1;
_bool CNavigation::m_bLineRender = true;
#endif

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
	if (FAILED(Read_Cell(pNavigationDataFilePath)))
		return E_FAIL;

	if (FAILED(SetUp_Neighbors()))
		return E_FAIL;

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
	if (m_pGameInstance->Key_Down('2') & 0x0001)
	{
		m_bLineRender = !m_bLineRender;

		m_iShaderPass =  m_bLineRender ? 1 : 0;
	}
#endif
}

std::string CNavigation::Make_Cell_Key(const _float3* fCellPoints) const
{
	std::vector<_float3> verts = { fCellPoints[0], fCellPoints[1], fCellPoints[2] };

	std::sort(verts.begin(), verts.end(),
		[](const _float3& a, const _float3& b)
		{
			if (a.x != b.x) return a.x < b.x;
			if (a.y != b.y) return a.y < b.y;
			return a.z < b.z;
		}
	);

	std::ostringstream oss;

	for (int i = 0; i < 3; ++i)
	{
		oss << static_cast<int>(verts[i].x * 1000) << "_"
			<< static_cast<int>(verts[i].y * 1000) << "_"
			<< static_cast<int>(verts[i].z * 1000);
		if (i < 2) oss << "_";
	}

	return oss.str();
}


HRESULT CNavigation::Make_Cell(const _float3* fCellPoints)
{
	// 키 생성
	std::string  CellKey = Make_Cell_Key(fCellPoints);

	// 이미 존재하는 삼각형이면 셀 생성 생략
	if (m_TriangleSet.find(CellKey) != m_TriangleSet.end())
		return S_OK;

	// 중복이 아니라면 키 추가
	m_TriangleSet.insert(CellKey);


	// 정렬된 포인트 배열 준비
	_float3  sortedPoints[3];
	Sort_Clockwise(fCellPoints, sortedPoints);

	// 파일 열기
	_ulong  dwByte = {};
	HANDLE  hFile = CreateFile(TEXT("../Bin/DataFiles/SoggySwamp_Navigation.dat"), GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	// 커서를 끝으로 옮겨서 이어쓰기
	SetFilePointer(hFile, 0, nullptr, FILE_END);

	// 1. 포인트 쓰기
	WriteFile(hFile, sortedPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	// 2. 문자열 길이 쓰기
	_uint  keyLength = static_cast<_uint>(CellKey.length());
	WriteFile(hFile, &keyLength, sizeof(_uint), &dwByte, nullptr);

	// 3. 문자열 내용 쓰기
	WriteFile(hFile, CellKey.c_str(), keyLength, &dwByte, nullptr);

	CloseHandle(hFile);


	// 시각화용 셀 생성하긔
	CCell* pCell = CCell::Create(m_pDevice, m_pContext, sortedPoints, static_cast<_int>(m_Cells.size()), CellKey);
	if (nullptr == pCell)
		return E_FAIL;

	m_Cells.push_back(pCell);

	//SetUp_Neighbors();

	std::cerr << "[셀 키 개수] : " << m_TriangleSet.size() << std::endl;

	return S_OK;
}

void CNavigation::Erase_Cell_Pick(const _matrix& WorldMatrixInverse)
{
	_float4  fWorldMousePos = {};
	_float3	 fWorldMouseRay = {};
	m_pGameInstance->Compute_MouseRay(fWorldMousePos, fWorldMouseRay);

	// 월드 -> 로컬 좌표로 마우스 정보 변환
	_vector		vLocalMousePos = XMVector3TransformCoord(XMLoadFloat4(&fWorldMousePos), WorldMatrixInverse);
	_vector		vLocalMouseRay = XMVector3TransformNormal(XMLoadFloat3(&fWorldMouseRay), WorldMatrixInverse);
	vLocalMouseRay = XMVector3Normalize(vLocalMouseRay);

	_float	fHitDistance = {};
	_float	fMinDist = FLT_MAX;
	_bool	bHit = false;

	CCell*  closestCell = nullptr;
	size_t  closestIndex = -1;

	for (size_t i = 0; i < m_Cells.size(); ++i)
	{
		_bool  bMeshHit = m_Cells[i]->Is_Picked(vLocalMousePos, vLocalMouseRay, fHitDistance);

		if (bMeshHit && fHitDistance < fMinDist)
		{
			fMinDist = fHitDistance;
			bHit = true;
			closestCell = m_Cells[i];
			closestIndex = i;
		}
	}

	if (bHit && closestCell != nullptr)
	{
		std::string  CellKey = closestCell->Get_CellKey();
		m_TriangleSet.erase(CellKey);
		std::cerr << "[셀 키 개수] : " << m_TriangleSet.size() << std::endl;
		// 메모리 해제
		Safe_Release(closestCell);

		// 벡터에서 제거
		m_Cells.erase(m_Cells.begin() + closestIndex);

		Resave_Files();

		std::cerr << "[셀 삭제됨] 남은 개수: " << m_Cells.size() << std::endl;
	}
}

void CNavigation::Erase_Cell_Last()
{
	if (m_Cells.empty())
		return;

	// 마지막 셀 가져오기
	CCell* pLastCell = m_Cells.back();

	// CellKey 추출해서 TriangleSet에서도 제거
	std::string CellKey = pLastCell->Get_CellKey();

	std::cerr << "[삭제 시도] 키: " << CellKey << std::endl;
	std::cerr << "[현재 키 수] " << m_TriangleSet.size() << std::endl;

	bool erased = m_TriangleSet.erase(CellKey) > 0;
	
	std::cerr << (erased ? "[삭제 성공]" : "[삭제 실패]") << std::endl;
	// 메모리 해제
	Safe_Release(pLastCell);

	// 벡터에서 제거
	m_Cells.pop_back();

	// 파일에 저장 상태 반영
	Resave_Files();

	std::cerr << "[셀 삭제됨] 남은 개수: " << m_Cells.size() << std::endl;
}


HRESULT CNavigation::Read_Cell(const _tchar* pNavigationDataFilePath)
{
	HANDLE		hFile = CreateFile(pNavigationDataFilePath, GENERIC_READ, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	while (true)
	{
		_float3		vPoints[3] = {};
		_ulong		dwByte = 0;

		// 포인트 읽기
		if (!ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr) || dwByte != sizeof(_float3) * 3)
			break;

		// 문자열 길이 읽기
		_uint keyLen = 0;
		if (!ReadFile(hFile, &keyLen, sizeof(_uint), &dwByte, nullptr) || dwByte != sizeof(_uint))
			break;

		// 문자열 내용 읽기
		std::vector<char> keyBuffer(keyLen + 1, 0);  // 문자열 길이만큼 char 배열 하나 만들고, 마지막에 \0 들어가게 +1 해줌
		if (!ReadFile(hFile, keyBuffer.data(), keyLen, &dwByte, nullptr) || dwByte != keyLen)
			break;

		std::string CellKey(keyBuffer.data());

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, static_cast<_int>(m_Cells.size()), CellKey);
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

	CloseHandle(hFile);

	return S_OK;
}

void CNavigation::Resave_Files()
{
	HANDLE hFile = CreateFile(TEXT("../Bin/DataFiles/SoggySwamp_Navigation.dat"),
		GENERIC_WRITE, 0, nullptr,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return;

	_ulong dwByte = 0;

	for (auto& pCell : m_Cells)
	{
		const _float3* pPoints = pCell->Get_Points();
		std::string key = pCell->Get_CellKey();

		_uint keyLen = static_cast<_uint>(key.length());

		WriteFile(hFile, pPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		WriteFile(hFile, &keyLen, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, key.c_str(), keyLen, &dwByte, nullptr);
	}

	CloseHandle(hFile);
}

void CNavigation::Sort_Clockwise(const _float3* pInPoints, _float3* pOutSorted)
{
	_float3 p0 = pInPoints[0];
	_float3 p1 = pInPoints[1];
	_float3 p2 = pInPoints[2];

	_vector edge1 = XMLoadFloat3(&p1) - XMLoadFloat3(&p0);
	_vector edge2 = XMLoadFloat3(&p2) - XMLoadFloat3(&p0);

	_vector vEdge1 = edge1;
	_vector vEdge2 = edge2;

	_vector vNormal = XMVector3Cross(vEdge1, vEdge2);
	vNormal = XMVector3Normalize(vNormal);

	// 위를 바라보는지 체크 (Y축 기준)
	if (XMVectorGetY(vNormal) < 0.f)
	{
		// 반시계방향이면 p1, p2 순서 바꾸기
		std::swap(p1, p2);
	}

	pOutSorted[0] = p0;
	pOutSorted[1] = p1;
	pOutSorted[2] = p2;
}

_bool CNavigation::Can_Move(_fvector vWorldPos)
{
	_matrix WorldMatrixInv = XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pWorldMatrix));
	_vector vLocalPos = XMVector3TransformCoord(vWorldPos, WorldMatrixInv);

	_int iCellIndex = m_iCurrentCellIndex;
	_int iNextNeighbor = -1;

	for (_int depth = 0; depth < 8; ++depth)
	{
		CCell* pCurrent = m_Cells[iCellIndex];

		if (pCurrent->Is_In(vLocalPos, &iNextNeighbor, nullptr))
		{
			_vector vNormal = XMLoadFloat3(&pCurrent->Get_PlaneNormal());

			// 평면이 걷기에 적당한지 확인
			if (XMVectorGetY(vNormal) < 0.5f)
			{	// 현재 평면이 벽면임

				if (iNextNeighbor != -1)
				{
					// 벽면과 인접한 셀 중 이동 가능한 평면 있는지 확인
					CCell* pNext = m_Cells[iNextNeighbor];
					_vector vNextNormal = XMLoadFloat3(&pNext->Get_PlaneNormal());

					if (XMVectorGetY(vNextNormal) >= 0.5f)
					{
						m_iCurrentCellIndex = iNextNeighbor;
						return true;
					}
				}

				break; // 벽면이거나 더 갈 수 없음
			}
			else
			{
				m_iCurrentCellIndex = iCellIndex;
				return true;
			}			
		}
		else
		{
			if (iNextNeighbor == -1)
				break;

			iCellIndex = iNextNeighbor;
		}
	}

	return false;
}

_bool CNavigation::Can_Slide(_fvector vPrevWorldPos, _fvector vMovingWorldPos, _vector& vSlidingPosition)
{
	_matrix		WorldMatrixInv = XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pWorldMatrix));
	_vector		vMovingLocalPos = XMVector3TransformCoord(vMovingWorldPos, WorldMatrixInv);
	_vector		vPrevLocalPos = XMVector3TransformCoord(vPrevWorldPos, WorldMatrixInv);

	_int  iCellIndex = m_iCurrentCellIndex;
	_int  iNextNeighbor = -1;
	_int  iHitEdgeIndex = -1;

	CCell* pCurrent = m_Cells[iCellIndex];

	if (false == pCurrent->Is_In(vMovingLocalPos, &iNextNeighbor, &iHitEdgeIndex))
	{
		if (iNextNeighbor == -1)
		{
			_vector vMoveDir = XMVector3Normalize(vMovingLocalPos - vPrevLocalPos);
			_vector vEdgeNormal = pCurrent->Get_EdgeNormal(iHitEdgeIndex);

			// 슬라이딩 벡터 계산
			_vector vSlideDir = vMoveDir - XMVector3Dot(vMoveDir, vEdgeNormal) * vEdgeNormal;
			vSlideDir = XMVector3Normalize(vSlideDir);  // ← 항상 정규화

			// 속도 보정
			const _float fSlideSpeed = 0.05f;  // 슬라이딩 이동 거리 (작게 조절)

			// 월드 기준으로 변환 + 이동 적용
			_vector vWorldSlideDir = XMVector3TransformNormal(vSlideDir, XMLoadFloat4x4(m_pWorldMatrix));
			_vector vSlideTargetWorld = vPrevWorldPos + vWorldSlideDir * fSlideSpeed;

			// 최대 10회 Is_In 재시도
			for (_int i = 0; i < 10; ++i)
			{
				_vector vSlideTargetLocal = XMVector3TransformCoord(vSlideTargetWorld, WorldMatrixInv);
				if (m_Cells[iCellIndex]->Is_In(vSlideTargetLocal, &iNextNeighbor, &iHitEdgeIndex))
				{
					vSlidingPosition = vSlideTargetWorld;
					m_iCurrentCellIndex = iCellIndex;
					return true;
				}
				else if (iNextNeighbor != -1)
				{
					iCellIndex = iNextNeighbor;
				}
				else
				{
					break;
				}
			}
		}
	}

	return false;
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
	if (nullptr == m_pWorldMatrix)
		return;

	_vector		vWorldPos = pTransform->Get_State(CTransform::STATE_POSITION);
	_matrix		WorldMatrixInv = XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pWorldMatrix));

	_vector		vPosition = XMVector3TransformCoord(vWorldPos, WorldMatrixInv);

	_float		fHeight   = m_Cells[m_iCurrentCellIndex]->Compute_Height(vPosition);

	vPosition = XMVectorSetY(vPosition, fHeight);

	pTransform->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(vPosition, XMLoadFloat4x4(m_pWorldMatrix)));
}

_bool CNavigation::Check_If_Grounded(CTransform* pObjectTransformCom)
{
	if (nullptr == m_pWorldMatrix)
		return false;

	if (m_iCurrentCellIndex < 0 || m_iCurrentCellIndex >= m_Cells.size())
		return false;

	_vector		vWorldPos = pObjectTransformCom->Get_State(CTransform::STATE_POSITION);  // 오브젝트 현재 월드 위치
	_matrix		WorldMatrixInv = XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pWorldMatrix));  // 맵의 월드 역행렬

	_vector		vObjectLocalPosition = XMVector3TransformCoord(vWorldPos, WorldMatrixInv);  // 오브젝트의 맵상 로컬 위치
	_float4		fObjectLocalPosition;
	XMStoreFloat4(&fObjectLocalPosition, vObjectLocalPosition);

	_float		fCellLocalHeight = m_Cells[m_iCurrentCellIndex]->Compute_Height(vObjectLocalPosition); // Map 기준 Cell 로컬 높이

	if (fObjectLocalPosition.y <= fCellLocalHeight)
		return true;

	return false;
}


#ifdef _DEBUG
HRESULT CNavigation::Render()
{
	if (nullptr == m_pWorldMatrix)
		return S_OK;

	_float4x4 WorldMatrix = *m_pWorldMatrix;

	/*if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;*/
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	_float4				vColor = {};

	if (-1 == m_iCurrentCellIndex)
	{
		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
			return E_FAIL;

		vColor = _float4(1.f, 0.f, 0.f, 1.f);

		if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
			return E_FAIL;

		m_pShader->Begin(m_iShaderPass);

		for (auto& pCell : m_Cells)
			pCell->Render();
	}
	else
	{
		WorldMatrix._42 += 0.1f; // 살짝 위로

		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
			return E_FAIL;

		vColor = _float4(0.f, 0.f, 1.f, 1.f);
		if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
			return E_FAIL;

		m_pShader->Begin(m_iShaderPass);

		m_Cells[m_iCurrentCellIndex]->Render();
	}

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

	m_pWorldMatrix = nullptr;
}
