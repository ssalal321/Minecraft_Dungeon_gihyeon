# CTransform 컴포넌트

## 1. 개요

**CTransform**은 모든 GameObject에 자동으로 부착되는 기본 컴포넌트로, 게임 오브젝트의 위치(position), 회전(rotation), 스케일(scale)을 하나의 World Matrix로 관리합니다.

### 설계 철학
- **표준화**: 모든 오브젝트가 동일한 방식으로 공간 변환 처리
- **중앙 집중식 관리**: World Matrix 하나로 모든 변환 정보 보관
- **성능 최적화**: 행렬 연산 캐싱 및 효율적인 공간 변환

---

## 2. 핵심 구조

### 2.1 World Matrix 구조

```cpp
class CTransform
{
private:
    _float4x4 m_WorldMatrix;  // Right, Up, Look, Position
    
public:
    enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION };
    
    _vector Get_State(STATE eState) const;
    void    Set_State(STATE eState, _fvector vState);
};
```

**World Matrix 구성:**
- `STATE_RIGHT`: 오른쪽 방향 벡터
- `STATE_UP`: 위쪽 방향 벡터  
- `STATE_LOOK`: 앞쪽 방향 벡터
- `STATE_POSITION`: 월드 공간 위치

### 2.2 자동 부착 메커니즘

```cpp
// 모든 GameObject 초기화 시 자동으로 Transform 추가
HRESULT CGameObject::Initialize(void* pArg)
{
    // Transform은 항상 자동 추가
    m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
    m_Components.emplace(g_strTransformTag, m_pTransformCom);
    m_pTransformCom->Initialize(pArg);
}
```

**효과:**
- GameObject는 항상 Transform 보유 보장
- Position/Rotation 접근을 일관된 인터페이스로 제공

---

## 3. 주요 기능

### 3.1 이동 시스템

```cpp
void CTransform::Go_Straight(_float fTimeDelta, 
                             CNavigation* pNavigation = nullptr, 
                             _float fSpeedFactor = 1.f)
{
    // 1. Look 벡터로 이동 방향 계산
    _vector vLook = Get_State(STATE_LOOK);
    _vector vNewPos = vPrevPos + XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta * fSpeedFactor;
    
    // 2. Navigation이 있으면 제약된 이동
    if (pNavigation)
    {
        if (pNavigation->Can_Move(vNewPos))
            Set_State(STATE_POSITION, vNewPos);  // 자유 이동
        else if (pNavigation->Can_Slide(...))
            Set_State(STATE_POSITION, vSlidingPos);  // 슬라이딩
    }
    else
    {
        Set_State(STATE_POSITION, vNewPos);  // 자유 이동
    }
}
```

**활용 예시:**
```cpp
// 플레이어 이동
pPlayer->Get_TransformCom()->Go_Straight(fTimeDelta, pNavigationCom, 1.0f);

// 스피드 바리에이션 적용
pMonster->Get_TransformCom()->Go_Straight(fTimeDelta, pNavigationCom, 0.5f);  // 느리게
```

### 3.2 회전 시스템

```cpp
void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
    // 축을 기준으로 회전
    _matrix RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);
    // World Matrix에 적용
}
```

### 3.3 점프 시스템

```cpp
void CTransform::Jump(_float fTimeDelta, CNavigation* pNavigation)
{
    // 중력 적용
    m_fJumpVelocity += m_fGravity * fTimeDelta;
    
    // Y축 이동
    _vector vPosition = Get_State(STATE_POSITION);
    vPosition.y += m_fJumpVelocity * fTimeDelta;
    
    // Navigation 위에 착지
    if (pNavigation && pNavigation->Check_If_Grounded(this))
    {
        m_fJumpVelocity = 0.f;
        m_bIsJumping = false;
    }
    
    Set_State(STATE_POSITION, vPosition);
}
```

---

## 4. 특수 활용: Parent-Child 관계

### 4.1 PartObject와의 통합

```cpp
// PartObject는 Parent의 World Matrix를 참조
class CPartObject : public CGameObject
{
protected:
    const _float4x4* m_pParentWorldMatrix;  // 부모 참조
    _float4x4 m_CombinedWorldMatrix;       // 최종 행렬
    
public:
    void Update()
    {
        // 부모의 World Matrix × 자신의 Local Transform
        _float4x4 Combined = ParentMatrix * LocalTransform;
        
        // 이 행렬로 렌더링/충돌 감지 등
    }
};
```

**실제 활용:**
```cpp
// Weapon PartObject는 Player의 Position을 따름
CPartObject* pWeapon = Player->Find_PartObject("Part_Weapon");

// Weapon의 최종 위치 = Player Position × Weapon Offset
_matrix FinalMatrix = PlayerWorldMatrix * WeaponLocalMatrix;
// → 무기가 플레이어의 손 위치에 자동 배치
```

### 4.2 소켓 시스템

```cpp
// Body의 본(Bone) 위치에서 Weapon 부착
CModel* pBodyModel = pBody->Find_Component("Com_Model");

// "J_R_Weapon" Bone의 변환 행렬 획득
_matrix SocketMatrix = pBodyModel->Get_CombinedTransformationMatrix("J_R_Weapon");

// Weapon은 이 소켓 행렬을 Parent로 참조
// → 정확한 손 위치에 무기 부착
```

---

## 5. Navigation 통합

### 5.1 선택적 제약

```cpp
void CTransform::Go_Straight(_float fTimeDelta, CNavigation* pNavigation, _float fSpeedFactor)
{
    if (pNavigation)
    {
        // Navigation이 있으면 제약된 이동
        if (pNavigation->Can_Move(vMovingPos))
            // 이동 허용
        else if (pNavigation->Can_Slide(...))
            // 슬라이딩 이동
    }
    else
    {
        // Navigation 없으면 자유 이동 (예: 화살)
    }
}
```

**시나리오별 적용:**
- **플레이어**: Navigation 있음 → 지형 제약 이동
- **화살**: Navigation 없음 → 자유로운 이동
- **비행 몬스터**: Navigation 없음 → 3D 공간 자유 이동

---

## 6. 실전 활용 사례

### 6.1 오브젝트별 Transform 사용

```cpp
// Player: Navigation 기반 이동
void CPlayer::Update(_float fTimeDelta)
{
    if (m_pNavigationCom)
        m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom, 1.0f);
}

// Arrow: 자유로운 이동
void CArrow::Update(_float fTimeDelta)
{
    // Navigation 없이 자유 이동
    m_pTransformCom->Go_Straight(fTimeDelta, nullptr, 1.0f);
}

// Monster: 조건부 Navigation
void CMonster::Update(_float fTimeDelta)
{
    if (m_pNavigationCom && !m_pTransformCom->Get_Is_Jumping())
    {
        m_pNavigationCom->SetUp_On_Navigation(m_pTransformCom);
    }
}
```

### 6.2 PartObject 계층 구조

```cpp
// Player (ContainerObject)
//  ├─ Transform (Player 위치)
//  └─ Body (PartObject)
//       ├─ Transform (Body 상대 위치)
//       └─ Weapon (PartObject)
//            └─ Transform (Weapon 소켓 위치)
//                 Parent: Player Transform × Body Socket

// 무기 교체 시 Weapon의 Transform만 변경하면 자동으로 올바른 위치에 배치
```

---

## 7. 성능 및 최적화

### 7.1 행렬 캐싱

```cpp
// Inverse Matrix 자동 캐싱
_matrix CTransform::Get_WorldMatrix_Inverse() const
{
    return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
}
```

### 7.2 조건부 업데이트

```cpp
// PartObject는 부모 World Matrix 참조만
void CPartObject::Update()
{
    // 부모 Matrix 참조만 업데이트
    // 실제 계산은 Render 시점에 수행
}
```

---

## 8. 핵심 가치

### 8.1 일관성
- 모든 오브젝트가 동일한 Transform 인터페이스 사용
- Position/Rotation 접근 방식 표준화

### 8.2 재사용성
- 이동/회전 로직을 한 번만 작성
- 모든 오브젝트에 자동 적용
- 버그 수정 시 모든 오브젝트에 자동 반영

### 8.3 유연성
- Navigation과 협력하여 제약된 이동
- PartObject 계층 구조 지원
- 소켓 시스템으로 정밀한 부착 위치 제어

### 8.4 확장성
- 새로운 이동 패턴 추가 시 Transform만 수정
- GameObject 코드 변경 불필요

---

## 9. 실제 코드 예시

### 9.1 기본 사용

```cpp
// Transform은 자동 추가
CGameObject* pObject = CreateObject();
CTransform* pTransform = pObject->Find_Component("Com_Transform");

// 위치 변경
pTransform->Set_State(STATE_POSITION, vNewPosition);

// 이동
pTransform->Go_Straight(fTimeDelta, pNavigation, 1.0f);

// 회전
pTransform->Turn(vAxis, fTimeDelta);
```

### 9.2 PartObject 활용

```cpp
// Weapon PartObject
CModel* pBody = pPlayer->Find_PartObject("Part_Body")->Find_Component("Com_Model");
_matrix SocketMatrix = pBody->Get_CombinedTransformationMatrix("J_R_Weapon");

// Weapon의 Transform은 SocketMatrix를 Parent로 참조
pWeapon->Set_SocketMatrix(SocketMatrix);
// → 무기가 정확한 손 위치에 자동 배치
```

---

## 10. 결론

CTransform 컴포넌트는 게임 오브젝트의 공간 변환을 중앙 집중식으로 관리하는 핵심 컴포넌트입니다. 
World Matrix 하나로 위치, 회전, 스케일을 효율적으로 관리하며, Navigation과의 통합 및 PartObject 계층 구조를 통해 복잡한 게임 오브젝트의 배치를 쉽게 구현할 수 있습니다.

**핵심 철학:**
- "모든 공간 변환은 Transform이 담당"
- "모든 오브젝트는 동일한 방식으로 Transform 관리"
- "PartObject는 Parent Transform을 자동 참조"

