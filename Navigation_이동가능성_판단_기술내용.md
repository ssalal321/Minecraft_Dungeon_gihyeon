# Navigation 이동 가능성 판단 시스템

## 1. 개요

Navigation 시스템에서 특정 위치로 이동할 수 있는지 판단하는 핵심 로직입니다. Triangle Cell 기반의 Navigation Mesh를 사용하여, 플레이어나 오브젝트가 이동 가능한 위치인지 실시간으로 검사합니다.

### 설계 목적
- **안전한 이동**: 이동 가능한 지형 위에서만 이동 허용
- **경사도 제한**: 너무 가파른 경사면은 이동 불가 처리
- **인접 셀 탐색**: 현재 셀에서 벗어나면 인접 셀로 자동 전환
- **성능 최적화**: 깊이 제한(depth limit)으로 탐색 범위 제한

---

## 2. 전체 호출 흐름

```
CTransform::Go_Straight()
    ↓
CNavigation::Can_Move(vMovingPosition)
    ↓
[월드 좌표 → 로컬 좌표 변환]
    ↓
[현재 셀부터 시작하여 인접 셀 탐색]
    ↓
CCell::Is_In(vLocalPos, ...)  // 셀 내부에 있는지 판단
    ↓
[평면 노말 확인 - 경사도 체크]
    ↓
[결과 반환: true/false]
```

---

## 3. 핵심 함수: CNavigation::Can_Move()

**위치**: `Engine/Private/Navigation.cpp` (336-387줄)

**역할**: 월드 좌표의 위치가 이동 가능한 셀 내부에 있는지 판단

**핵심 코드**:
```cpp
_bool CNavigation::Can_Move(_fvector vWorldPos)
{
    // 1. 월드 좌표 → 로컬 좌표 변환
    _matrix WorldMatrixInv = XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pWorldMatrix));
    _vector vLocalPos = XMVector3TransformCoord(vWorldPos, WorldMatrixInv);

    _int iCellIndex = m_iCurrentCellIndex;  // 현재 셀 인덱스
    _int iNextNeighbor = -1;

    // 2. 최대 8 depth까지 인접 셀 탐색
    for (_int depth = 0; depth < 8; ++depth)
    {
        CCell* pCurrent = m_Cells[iCellIndex];

        // 3. 현재 셀 내부에 위치가 있는지 확인
        if (pCurrent->Is_In(vLocalPos, &iNextNeighbor, nullptr))
        {
            _vector vNormal = XMLoadFloat3(&pCurrent->Get_PlaneNormal());

            // 4. 경사도 확인 (Y 성분이 0.5 이상이어야 이동 가능)
            if (XMVectorGetY(vNormal) < 0.5f)
            {   // 경사가 너무 가파름

                // 5. 인접 셀로 이동 가능한지 재확인
                if (iNextNeighbor != -1)
                {
                    CCell* pNext = m_Cells[iNextNeighbor];
                    _vector vNextNormal = XMLoadFloat3(&pNext->Get_PlaneNormal());

                    // 인접 셀의 경사도가 적절하면 이동 허용
                    if (XMVectorGetY(vNextNormal) >= 0.5f)
                    {
                        m_iCurrentCellIndex = iNextNeighbor;
                        return true;
                    }
                }

                break; // 경사가 가파르거나 이동할 셀이 없음
            }
            else
            {
                // 6. 이동 가능한 경우 현재 셀 인덱스 업데이트
                m_iCurrentCellIndex = iCellIndex;
                return true;
            }			
        }
        else
        {
            // 7. 현재 셀 밖에 있으면 인접 셀로 이동
            if (iNextNeighbor == -1)
                break;  // 인접 셀이 없으면 종료

            iCellIndex = iNextNeighbor;  // 다음 셀로 이동
        }
    }

    return false;  // 이동 불가능
}
```

---

## 4. 셀 내부 판단: CCell::Is_In()

**위치**: `Engine/Private/Cell.cpp` (62-78줄)

**역할**: 특정 위치가 삼각형 셀 내부에 있는지 판단

**알고리즘**: Half-space 테스트 (반공간 판정)

**핵심 코드**:
```cpp
_bool CCell::Is_In(_fvector vPosition, _int* pNeighborIndex, _int* pHitEdgeIndex)
{
    // 삼각형의 각 변(AB, BC, CA)에 대해 테스트
    for (size_t i = 0; i < LINE_END; i++)
    {
        // 위치에서 각 꼭짓점으로의 벡터
        _vector vDir = vPosition - XMLoadFloat3(&m_vPoints[i]);

        // 변의 외부 방향 노말과 내적 계산
        // 내적 > 0 이면 외부에 있음
        if (0 < XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir), XMLoadFloat3(&m_vNormals[i]))))
        {
            // 외부에 있으면 인접 셀 인덱스 반환
            *pNeighborIndex = m_iNeighborCellIndices[i];
            if (pHitEdgeIndex)
                *pHitEdgeIndex = static_cast<_int>(i);

            return false;  // 셀 밖에 있음
        }
    }
    return true;  // 모든 변의 안쪽에 있으면 셀 내부
}
```

### Half-space 테스트 원리

```
         C
         /\
        /  \
       /    \
      /  P   \    <- P는 셀 내부
     /        \
    /__________\
   A            B

각 변(AB, BC, CA)에 대해:
1. 변의 외부 방향 노말 계산 (셀 초기화 시)
2. 위치 P에서 각 꼭짓점으로의 벡터 계산
3. 벡터와 노말의 내적 계산
4. 내적 > 0 → 외부, 내적 <= 0 → 내부

모든 변에 대해 내적 <= 0이면 셀 내부!
```

### Edge Normal 계산

**위치**: `Engine/Private/Cell.cpp` (19-26줄)

```cpp
// 각 변의 방향 벡터 계산
_float3 vLines[LINE_END] = {};
XMStoreFloat3(&vLines[LINE_AB], 
    XMVector3Normalize(XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_A])));
// ... (BC, CA 동일)

// 외부 방향 노말 계산 (Y=0 평면에서의 외부 방향)
for (size_t i = 0; i < LINE_END; i++)
    m_vNormals[i] = _float3(-vLines[i].z, 0.f, vLines[i].x);
```

---

## 5. 경사도 판단 로직

### 5.1 Plane Normal 확인

각 셀은 평면(Plane)을 가지며, 이 평면의 노말(Normal)을 계산합니다.

**셀 초기화** (`Engine/Private/Cell.cpp` 30-43줄):
```cpp
// 3개의 점으로부터 평면 방정식 계산
XMStoreFloat4(&m_vPlane,
    XMPlaneFromPoints(
        XMVectorSetW(XMLoadFloat3(&m_vPoints[POINT_A]), 1.f),
        XMVectorSetW(XMLoadFloat3(&m_vPoints[POINT_B]), 1.f),
        XMVectorSetW(XMLoadFloat3(&m_vPoints[POINT_C]), 1.f)));

// 평면 노말 추출
XMStoreFloat3(&m_vPlaneNormal, XMVector3Normalize(XMLoadFloat4(&m_vPlane)));
```

**평면 방정식**: `ax + by + cz + d = 0`
- `(a, b, c)` = 평면 노말 벡터
- `d` = 평면 오프셋

### 5.2 경사도 임계값

```cpp
if (XMVectorGetY(vNormal) < 0.5f)
{
    // 경사가 너무 가파름 → 이동 불가
}
else
{
    // 경사가 적절함 → 이동 가능
}
```

**의미**:
- `Y >= 0.5f`: 경사가 완만함 (약 60도 이하) → 이동 가능
- `Y < 0.5f`: 경사가 가파름 (약 60도 이상) → 이동 불가

**Y 성분 값의 의미**:
- `Y = 1.0`: 완전히 수평 (평지)
- `Y = 0.707`: 45도 경사
- `Y = 0.5`: 약 60도 경사
- `Y = 0.0`: 완전히 수직 (벽)

---

## 6. 인접 셀 탐색 전략

### 6.1 Depth-limited Search

```cpp
for (_int depth = 0; depth < 8; ++depth)
{
    // 현재 셀에서 시작하여 최대 8 depth까지 탐색
    // 깊이 제한으로 무한 루프 방지 및 성능 최적화
}
```

**왜 8 depth인가?**
- 대부분의 경우 현재 셀이나 인접 셀 1-2개 내에서 해결
- 8 depth는 충분히 넓은 범위 커버
- 너무 깊게 탐색하면 성능 저하

### 6.2 인접 셀 네트워크

각 셀은 3개의 변을 가지며, 각 변에 대해 인접 셀 정보를 저장합니다:

```cpp
// CCell.h
_int m_iNeighborCellIndices[LINE_END] = { -1, -1, -1 };
// LINE_AB, LINE_BC, LINE_CA 각각에 대한 인접 셀 인덱스
```

**인접 셀 설정** (`SetUp_Neighbors` 함수):
- Navigation 초기화 시 모든 셀의 인접 관계 설정
- 변을 공유하는 셀들을 연결

---

## 7. 좌표계 변환

### 7.1 월드 → 로컬 변환

```cpp
_matrix WorldMatrixInv = XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pWorldMatrix));
_vector vLocalPos = XMVector3TransformCoord(vWorldPos, WorldMatrixInv);
```

**이유**:
- Navigation Mesh의 셀 데이터는 로컬 좌표로 저장됨
- 월드 좌표를 로컬 좌표로 변환해야 셀과 비교 가능
- 맵 오브젝트의 Transform이 변경되어도 자동 대응

---

## 8. 실제 사용 예시

### 8.1 CTransform::Go_Straight()에서 사용

**위치**: `Engine/Private/Transform.cpp` (58-82줄)

```cpp
void CTransform::Go_Straight(_float fTimeDelta, CNavigation* pNavigation, _float fSpeedFactor)
{
    _vector  vLook = Get_State(STATE_LOOK);
    _vector  vPrevPosition = Get_State(STATE_POSITION);
    _vector  vMovingPosition = vPrevPosition + 
        XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta * fSpeedFactor;

    _vector  vSlidingPosition = vPrevPosition;

    if (nullptr != pNavigation)
    {
        // 이동 가능한 위치인지 확인
        if (pNavigation->Can_Move(vMovingPosition))
        {
            Set_State(STATE_POSITION, vMovingPosition);
        }
        // 이동 불가능하면 미끄러지기 시도
        else if (pNavigation->Can_Slide(vPrevPosition, vMovingPosition, vSlidingPosition))
        {
            Set_State(STATE_POSITION, vSlidingPosition);
        }
        // 둘 다 실패하면 이동하지 않음 (현재 위치 유지)
    }
    else
    {
        // Navigation 없으면 자유 이동
        Set_State(STATE_POSITION, vMovingPosition);
    }
}
```

---

## 9. 알고리즘 플로우 차트

```
시작: Can_Move(월드 좌표)
    ↓
[월드 좌표 → 로컬 좌표 변환]
    ↓
[현재 셀 인덱스 가져오기]
    ↓
depth = 0부터 8까지 반복
    ↓
[현재 셀의 Is_In() 호출]
    ↓
┌─────────────────────┐
│ 위치가 셀 내부인가? │
└─────────────────────┘
    ↓
   [YES]              [NO]
    ↓                  ↓
[평면 노말 확인]   [인접 셀로 이동]
    ↓                  ↓
[Y >= 0.5?]        [인접 셀 존재?]
    ↓                  ↓
   [YES]  [NO]      [YES]  [NO]
    ↓      ↓          ↓      ↓
[성공]  [인접 확인] [이동] [실패]
    ↓      ↓          ↓
    └──────┴──────────┘
         ↓
      [성공/실패 반환]
```

---

## 10. 성능 최적화 기법

### 10.1 Depth Limit
- 최대 8 depth까지만 탐색하여 무한 루프 방지
- 대부분의 경우 1-2 depth 내에서 해결

### 10.2 현재 셀 우선 탐색
- `m_iCurrentCellIndex`에서 시작
- 대부분의 경우 첫 번째 시도에서 성공

### 10.3 Half-space 테스트
- 빠른 수학 연산 (내적 계산)
- 삼각형의 3개 변에 대해만 테스트

---

## 11. 주요 특징

### 11.1 경사도 제한
- **임계값**: Y 성분 >= 0.5 (약 60도 이하)
- **효과**: 가파른 벽이나 경사면 이동 불가
- **현실감**: 실제 물리 기반 이동 제약

### 11.2 인접 셀 자동 전환
- 현재 셀 밖으로 나가면 자동으로 인접 셀 탐색
- 플레이어가 셀 경계를 넘어도 자연스러운 이동

### 11.3 로컬 좌표 기반
- 맵의 Transform 변경에 유연하게 대응
- 셀 데이터 재사용 가능

---

## 12. 핵심 정리

1. **좌표 변환**: 월드 좌표 → 로컬 좌표 (InvWorldMatrix 사용)
2. **셀 탐색**: 현재 셀부터 시작하여 최대 8 depth까지 인접 셀 탐색
3. **내부 판단**: Half-space 테스트로 셀 내부 여부 확인
4. **경사도 체크**: 평면 노말의 Y 성분 >= 0.5 여부 확인
5. **인접 셀 전환**: 셀 밖이면 인접 셀로 자동 이동
6. **결과 반환**: 이동 가능하면 true, 현재 셀 인덱스 업데이트

**핵심 철학:**
- "현재 셀 우선 탐색으로 빠른 판단"
- "경사도 제한으로 현실적인 이동 제약"
- "인접 셀 네트워크로 자연스러운 이동 경계 처리"
- "Depth limit으로 성능과 정확도의 균형"

---

## 13. 관련 함수

### 13.1 Can_Slide()
- 이동 불가능한 경우 벽을 따라 미끄러지는 로직
- Edge Normal을 이용한 반사 벡터 계산

### 13.2 SetUp_On_Navigation()
- Transform이 Navigation 위에 정확히 위치하도록 설정
- 셀의 높이를 계산하여 Y 좌표 조정

### 13.3 Check_If_Grounded()
- 오브젝트가 Navigation 위에 착지했는지 확인
- 점프 후 착지 판단에 사용

