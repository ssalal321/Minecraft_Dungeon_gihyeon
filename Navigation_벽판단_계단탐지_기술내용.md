# Navigation 벽 판단 및 계단 탐지 시스템

## 1. 개요

Navigation 시스템에서 벽(WALL)과 보행 가능한 지형(WALKABLE)을 구분하고, 벽이더라도 계단일 가능성이 있는 경우를 탐지하는 로직입니다.

### 설계 목적
- **벽 판단**: 가파른 경사면이나 수직 벽을 이동 불가능한 지형으로 분류
- **계단 탐지**: 벽이더라도 인접한 보행 가능한 지형이 있으면 계단으로 인식
- **자연스러운 이동**: 플레이어가 계단을 오를 수 있도록 네비게이션 메시 위에 배치

---

## 2. 전체 처리 흐름

```
플레이어 이동 시도
    ↓
CNavigation::Can_Move(vWorldPos)
    ↓
CNavigation::Decided_Next_Index()
    ↓
┌─────────────────────────────────┐
│ [1단계] 현재 셀 분류              │
│ Classify_Cell(pCurCell)          │
│   ↓                              │
│ Is_Wall(normal)                  │
│   ↓                              │
│ normal.y < 0.5f?                 │
└─────────────────────────────────┘
    ↓
┌─────────────────────────────────┐
│ [2단계] 계단 탐지 로직            │
│ 현재 셀이 WALL이지만             │
│ 인접 셀이 WALKABLE인가?          │
└─────────────────────────────────┘
    ↓
┌─────────────────────────────────┐
│ [3단계] 플레이어 배치             │
│ SetUp_On_Navigation()           │
│ - 네비게이션 메시 위에 위치 설정 │
│ - 셀의 높이 계산하여 Y 좌표 조정 │
└─────────────────────────────────┘
```

---

## 3. 벽 판단 로직

### 3.1 Is_Wall() 함수

**위치**: `Engine/Public/Navigation.h` (72-75줄)

**코드**:
```cpp
_bool Is_Wall(const _float3& normal) const 
{
    return (normal.y < 0.5f);
}
```

**역할**: 평면의 노말 벡터를 받아서 벽인지 판단

**판단 기준**:
- `normal.y < 0.5f` → **벽 (WALL)**
- `normal.y >= 0.5f` → **보행 가능 (WALKABLE)**

### 3.2 Y 성분의 의미

평면 노말 벡터의 Y 성분은 **평면이 수평면과 얼마나 가까운지**를 나타냅니다.

```
        Y = 1.0
        │
        │  완전히 수평 (평지)
        │
        │  Y = 0.707 (45도 경사)
        │
        │  Y = 0.5 (약 60도 경사) ← 임계값
        │
        │  Y = 0.0
        │
        └─────────────────────────
        완전히 수직 (벽)
```

**수학적 의미**:
- 노말 벡터 = `(a, b, c)` (정규화된 벡터, 길이 = 1)
- Y 성분 `b`는 수직 방향 성분
- `b = cos(θ)` (θ는 평면과 수평면의 각도)

**임계값 0.5의 의미**:
- `cos(60°) ≈ 0.5`
- 즉, **약 60도 이상의 경사면**은 벽으로 판단
- 플레이어가 올라갈 수 없는 가파른 경사면

### 3.3 왜 Y 성분만 확인하는가?

**2D 평면에서의 경사도 판단**:
- XZ 평면(지면)에 대한 경사도는 Y 성분만으로 판단 가능
- Y 성분이 클수록 → 수평에 가까움 → 이동 가능
- Y 성분이 작을수록 → 수직에 가까움 → 이동 불가

**예시**:
```cpp
// 평지 (0도 경사)
normal = (0, 1.0, 0)  → Y = 1.0  → WALKABLE ✅

// 45도 경사
normal = (0.707, 0.707, 0)  → Y = 0.707  → WALKABLE ✅

// 60도 경사 (임계값)
normal = (0.866, 0.5, 0)  → Y = 0.5  → WALKABLE ✅

// 70도 경사
normal = (0.94, 0.342, 0)  → Y = 0.342  → WALL ❌

// 수직 벽 (90도)
normal = (1.0, 0.0, 0)  → Y = 0.0  → WALL ❌
```

---

## 4. Classify_Cell() 함수

**위치**: `Engine/Private/Navigation.cpp` (418-426줄)

**코드**:
```cpp
CNavigation::WALKABLE_TYPE CNavigation::Classify_Cell(CCell* pCell) const
{
    if (pCell == nullptr) 
        return INVALID;

    const _float3& normal = pCell->Get_PlaneNormal();

    return Is_Wall(normal) ? WALL : WALKABLE;
}
```

**역할**: 셀의 타입을 분류 (WALKABLE / WALL / INVALID)

**처리 과정**:
1. 셀의 평면 노말 벡터 가져오기
2. `Is_Wall()` 함수로 벽 여부 판단
3. 결과에 따라 WALL 또는 WALKABLE 반환

**반환값**:
- `WALKABLE`: 보행 가능한 지형 (경사가 완만함)
- `WALL`: 벽 또는 가파른 경사면 (이동 불가)
- `INVALID`: 유효하지 않은 셀 (nullptr)

---

## 5. 계단 탐지 로직

### 5.1 Decided_Next_Index() 함수

**위치**: `Engine/Private/Navigation.cpp` (393-416줄)

**코드**:
```cpp
_bool CNavigation::Decided_Next_Index(_int iCurIndex, _int iNeighborIndex, _int* pOutNextIndex)
{
    if (pOutNextIndex == nullptr)
        return false;

    CCell* pCurCell = m_Cells[iCurIndex];
    CCell* pNextCell = (iNeighborIndex != -1) ? m_Cells[iNeighborIndex] : nullptr;

    // [1단계] 현재 셀이 보행 가능한가?
    if (Classify_Cell(pCurCell) == WALKABLE)
    {
        *pOutNextIndex = iCurIndex;      // 현재 셀 사용 OK
        return true;
    }

    // [2단계] 현재 셀이 벽이지만, 인접 셀이 보행 가능한가? (계단 탐지)
    if (iNeighborIndex != -1 && Classify_Cell(pNextCell) == WALKABLE)
    {
        *pOutNextIndex = iNeighborIndex; // 인접 셀로 이동 (계단)
        return true;
    }

    // [3단계] 둘 다 실패
    return false;
}
```

### 5.2 계단 탐지 원리

**시나리오**: 플레이어가 계단을 올라가는 경우

```
    ┌─────────┐
    │  WALK   │ ← 인접 셀 (보행 가능)
    │  -ABLE  │
    └─────────┘
         │
    ┌────┴────┐
    │  WALL   │ ← 현재 셀 (벽, 가파른 경사)
    │  (계단) │
    └─────────┘
```

**처리 과정**:
1. **현재 셀 확인**: `Classify_Cell(pCurCell)` → `WALL`
2. **인접 셀 확인**: `Classify_Cell(pNextCell)` → `WALKABLE`
3. **계단 판단**: 벽이지만 인접 셀이 보행 가능 → 계단으로 인식
4. **결과**: 인접 셀로 이동 허용 (`iNeighborIndex` 반환)

**왜 이렇게 동작하는가?**
- 계단의 각 단은 **가파른 경사면** (벽으로 분류)
- 하지만 계단의 **다음 단은 보행 가능**한 지형
- 따라서 벽이더라도 인접 셀이 보행 가능하면 계단으로 판단

### 5.3 실제 계단 예시

```
계단 구조:
    
    ┌─────┐  ← 셀 3 (WALKABLE)
    │     │
    ├─────┤  ← 셀 2 (WALKABLE) 
    │     │
    ├─────┤  ← 셀 1 (WALL - 계단면)
    │     │
    └─────┘  ← 셀 0 (WALKABLE)

플레이어가 셀 1 위에 있을 때:
- 셀 1은 WALL (가파른 경사)
- 하지만 셀 2가 인접하고 WALKABLE
- → 계단으로 인식하여 셀 2로 이동 허용
```

---

## 6. SetUp_On_Navigation() 함수

### 6.1 함수 개요

**위치**: `Engine/Private/Navigation.cpp` (516-531줄)

**역할**: 플레이어를 네비게이션 메시 위에 정확히 배치

**코드**:
```cpp
void CNavigation::SetUp_On_Navigation(CTransform* pTransform)
{
    if (nullptr == m_pWorldMatrix)
        return;

    // [1단계] 플레이어의 월드 좌표 가져오기
    _vector vWorldPos = pTransform->Get_State(CTransform::STATE_POSITION);
    
    // [2단계] 월드 좌표 → 로컬 좌표 변환
    _matrix WorldMatrixInv = XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pWorldMatrix));
    _vector vPosition = XMVector3TransformCoord(vWorldPos, WorldMatrixInv);

    // [3단계] 현재 셀의 높이 계산
    _float fHeight = m_Cells[m_iCurrentCellIndex]->Compute_Height(vPosition);

    // [4단계] Y 좌표를 셀의 높이로 설정
    vPosition = XMVectorSetY(vPosition, fHeight);

    // [5단계] 로컬 좌표 → 월드 좌표 변환 후 Transform에 적용
    pTransform->Set_State(CTransform::STATE_POSITION, 
        XMVector3TransformCoord(vPosition, XMLoadFloat4x4(m_pWorldMatrix)));
}
```

### 6.2 단계별 상세 설명

#### 1단계: 플레이어 위치 가져오기
```cpp
_vector vWorldPos = pTransform->Get_State(CTransform::STATE_POSITION);
```
- Transform 컴포넌트에서 현재 월드 좌표 가져오기

#### 2단계: 좌표 변환
```cpp
_matrix WorldMatrixInv = XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pWorldMatrix));
_vector vPosition = XMVector3TransformCoord(vWorldPos, WorldMatrixInv);
```
- 월드 좌표를 로컬 좌표로 변환
- Navigation Mesh의 Transform에 맞춰 변환

#### 3단계: 높이 계산
```cpp
_float fHeight = m_Cells[m_iCurrentCellIndex]->Compute_Height(vPosition);
```

**Compute_Height() 함수** (`Engine/Private/Cell.cpp` 127-133줄):
```cpp
_float CCell::Compute_Height(_fvector vPosition)
{
    /* 평면 방정식: ax + by + cz + d = 0 */
    /* y = (-ax - cz - d) / b */
    
    return (-m_vPlane.x * XMVectorGetX(vPosition) 
            - m_vPlane.z * XMVectorGetZ(vPosition) 
            - m_vPlane.w) / m_vPlane.y;
}
```

**원리**:
- 평면 방정식: `ax + by + cz + d = 0`
- Y에 대해 정리: `y = (-ax - cz - d) / b`
- X, Z 좌표가 주어졌을 때 평면 위의 Y 좌표 계산

#### 4단계: Y 좌표 설정
```cpp
vPosition = XMVectorSetY(vPosition, fHeight);
```
- X, Z 좌표는 유지하고 Y 좌표만 셀의 높이로 설정
- 플레이어가 셀 위에 정확히 위치하도록 함

#### 5단계: 좌표 변환 및 적용
```cpp
pTransform->Set_State(CTransform::STATE_POSITION, 
    XMVector3TransformCoord(vPosition, XMLoadFloat4x4(m_pWorldMatrix)));
```
- 로컬 좌표를 다시 월드 좌표로 변환
- Transform에 적용하여 플레이어 위치 업데이트

### 6.3 계단 타는 동작 구현

**전체 흐름**:
```
1. 플레이어 이동 시도
   ↓
2. Can_Move() 호출
   ↓
3. Decided_Next_Index()에서 계단 탐지
   - 현재 셀: WALL
   - 인접 셀: WALKABLE
   - → 계단으로 인식, 인접 셀로 이동 허용
   ↓
4. SetUp_On_Navigation() 호출
   - 플레이어를 네비게이션 메시 위에 배치
   - 셀의 높이 계산하여 Y 좌표 조정
   - → 계단을 따라 자연스럽게 올라감
```

**시각적 예시**:
```
Before SetUp_On_Navigation():
    플레이어 위치: (x, y_old, z)
                     ↓
    셀 높이 계산: Compute_Height(x, z) = y_new
                     ↓
After SetUp_On_Navigation():
    플레이어 위치: (x, y_new, z)  ← 셀 위에 정확히 배치
```

---

## 7. 벽 판단 방법의 한계와 개선 가능성

### 7.1 현재 방법의 한계

**현재 구현**:
```cpp
_bool Is_Wall(const _float3& normal) const 
{
    return (normal.y < 0.5f);
}
```

**문제점**:
1. **단순한 임계값**: Y 성분만으로 판단
2. **고정된 임계값**: 모든 상황에 동일한 0.5 사용
3. **방향 무시**: 평면의 방향(위/아래) 구분 없음

### 7.2 개선 가능한 방법들

#### 방법 1: 각도 기반 판단
```cpp
_bool Is_Wall(const _float3& normal) const 
{
    // Y 성분을 각도로 변환
    _float angle = acosf(normal.y) * (180.0f / XM_PI);
    return (angle > 60.0f);  // 60도 이상이면 벽
}
```

#### 방법 2: 경사도 계산
```cpp
_bool Is_Wall(const _float3& normal) const 
{
    // 경사도 = sqrt(1 - y^2) / y
    _float slope = sqrtf(1.0f - normal.y * normal.y) / normal.y;
    return (slope > 1.732f);  // tan(60°) ≈ 1.732
}
```

#### 방법 3: 설정 가능한 임계값
```cpp
class CNavigation {
private:
    _float m_fWallThreshold = 0.5f;  // 임계값을 멤버 변수로

public:
    _bool Is_Wall(const _float3& normal) const 
    {
        return (normal.y < m_fWallThreshold);
    }
    
    void Set_WallThreshold(_float threshold) {
        m_fWallThreshold = threshold;
    }
};
```

### 7.3 현재 방법이 적절한 이유

**장점**:
1. **간단하고 빠름**: 단순 비교 연산
2. **충분한 정확도**: 대부분의 게임 시나리오에서 적절
3. **성능 최적화**: 복잡한 계산 없음

**적용 분야**:
- 3D 게임의 네비게이션 메시
- 플레이어 이동 제약
- AI 경로 탐색

---

## 8. 전체 동작 시나리오

### 시나리오 1: 평지 이동
```
1. 플레이어 위치: (10, 5, 10)
2. Can_Move() 호출
3. 현재 셀: WALKABLE (normal.y = 0.9)
4. Decided_Next_Index() → 현재 셀 사용
5. SetUp_On_Navigation() → Y 좌표 조정
6. 결과: 정상 이동 ✅
```

### 시나리오 2: 계단 오르기
```
1. 플레이어 위치: (10, 5, 10)
2. Can_Move() 호출
3. 현재 셀: WALL (normal.y = 0.3) ← 계단면
4. 인접 셀: WALKABLE (normal.y = 0.8) ← 다음 단
5. Decided_Next_Index() → 인접 셀로 이동 (계단 탐지)
6. SetUp_On_Navigation() → 계단 높이에 맞춰 Y 좌표 조정
7. 결과: 계단을 따라 자연스럽게 올라감 ✅
```

### 시나리오 3: 벽 충돌
```
1. 플레이어 위치: (10, 5, 10)
2. Can_Move() 호출
3. 현재 셀: WALL (normal.y = 0.2)
4. 인접 셀: 없음 또는 WALL
5. Decided_Next_Index() → 실패
6. 결과: 이동 불가 ❌
```

---

## 9. 핵심 정리

### 9.1 벽 판단 (`Is_Wall()`)
- **기준**: 평면 노말의 Y 성분 < 0.5
- **의미**: 약 60도 이상의 경사면을 벽으로 판단
- **장점**: 간단하고 빠른 판단

### 9.2 계단 탐지 (`Decided_Next_Index()`)
- **로직**: 현재 셀이 벽이지만 인접 셀이 보행 가능하면 계단으로 인식
- **효과**: 플레이어가 계단을 자연스럽게 올라갈 수 있음

### 9.3 플레이어 배치 (`SetUp_On_Navigation()`)
- **역할**: 플레이어를 네비게이션 메시 위에 정확히 배치
- **방법**: 셀의 평면 방정식으로 높이 계산 후 Y 좌표 조정
- **효과**: 계단을 따라 자연스럽게 이동

---

## 10. 관련 함수 참조

- `Classify_Cell()`: 셀 타입 분류 (WALKABLE/WALL/INVALID)
- `Is_Wall()`: 벽 여부 판단
- `Decided_Next_Index()`: 이동 가능한 셀 선택 (계단 탐지 포함)
- `SetUp_On_Navigation()`: 플레이어를 네비게이션 메시 위에 배치
- `Compute_Height()`: 셀의 평면 방정식으로 높이 계산

---

## 11. 주의사항

1. **임계값 조정**: 게임에 따라 0.5 임계값을 조정할 수 있음
   - 더 가파른 경사 허용: 0.3 등으로 낮춤
   - 더 완만한 경사만 허용: 0.7 등으로 높임

2. **인접 셀 설정**: `SetUp_Neighbors()`가 올바르게 호출되어야 계단 탐지가 작동

3. **현재 셀 인덱스**: `m_iCurrentCellIndex`가 올바르게 유지되어야 함

4. **좌표 변환**: 월드 매트릭스가 올바르게 설정되어야 정확한 계산 가능

