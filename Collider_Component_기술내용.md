# Collider Component 기술소개서

## 개요

`CCollider`는 게임 오브젝트에 충돌 감지 기능을 제공하는 컴포넌트입니다. `CBounding` 시스템을 활용하여 AABB, OBB, Sphere 세 가지 타입의 충돌 감지를 지원하며, `CCollision_Manager`와 협력하여 효율적인 충돌 검사를 수행합니다. 충돌 상태(ENTER/STAY/EXIT)를 추적하고, GameObject에 충돌 이벤트를 전달하여 게임 로직과 충돌 감지를 완전히 분리합니다.

## 아키텍처

### 클래스 구조

```
CComponent (기본 클래스)
    └── CCollider (최종 클래스)
        ├── CBounding (추상 클래스)
        │   ├── CBounding_AABB
        │   ├── CBounding_OBB
        │   └── CBounding_Sphere
        └── CGameObject (소유자)
```

### 핵심 인터페이스

```cpp
class CCollider final : public CComponent
{
public:
    enum COLLISION_STATE { ENTER, STAY, EXIT, STATE_END };
    enum COLLIDER_ROLE { SMALL, BIG, ETC };
    
    // 충돌 검사
    _bool Intersect(CCollider* pTargetCollider);
    
    // 충돌 상태 관리
    void Collided_With(CCollider* pOther);
    void Process_Collisions();
    
    // 업데이트
    void Update();
    
#ifdef _DEBUG
    HRESULT Render();  // 디버그 렌더링
#endif
};
```

---

## 핵심 기능

### 1. Bounding 기반 충돌 감지

Collider는 내부적으로 `CBounding` 객체를 보유하여 실제 충돌 검사를 수행합니다.

```cpp
HRESULT CCollider::Initialize(void* pArg)
{
    CBounding::BOUNDING_DESC* pDesc = static_cast<CBounding::BOUNDING_DESC*>(pArg);

    switch (m_eColliderType)
    {
    case COLLIDER_TYPE::TYPE_AABB:
        m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, pDesc);
        break;
    case COLLIDER_TYPE::TYPE_OBB:
        m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext, pDesc);
        break;
    case COLLIDER_TYPE::TYPE_SPHERE:
        m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext, pDesc);
        break;
    }
    
    // 소유자 정보 저장
    m_CombinedWorldMatrix = pDesc->CombinedWorldMatrix;
    m_pOwnerGameObject = pDesc->pGameObject;
    m_bCollisionActivated = pDesc->pCollisionActivated;
    
    return S_OK;
}
```

**설계 원칙:**
- Collider는 Bounding 타입에 따라 적절한 Bounding 객체를 생성합니다.
- Bounding의 구체적인 충돌 검사 로직은 Collider가 직접 처리하지 않고, Bounding에 위임합니다.
- 이로 인해 Bounding 타입이 추가되어도 Collider 코드 수정이 최소화됩니다.

### 2. Transform과의 연동

Collider는 GameObject의 Transform 정보를 참조하여 매 프레임 Bounding을 업데이트합니다.

```cpp
void CCollider::Update()
{
    if (!m_bColliderActive)
        return;

    // GameObject의 Combined World Matrix를 가져옴
    _matrix mCombinedWorldMatrix = XMLoadFloat4x4(m_CombinedWorldMatrix);
    
    // Bounding의 위치/회전/스케일을 업데이트
    m_pBounding->Update(mCombinedWorldMatrix);
}
```

**특징:**
- `m_CombinedWorldMatrix`는 GameObject의 Transform에서 제공되는 포인터입니다.
- Collider는 Transform의 내부 구현을 알 필요 없이 World Matrix만 참조합니다.
- `m_bColliderActive` 플래그로 런타임에 충돌 감지를 활성/비활성화할 수 있습니다.

---

## 충돌 검사 시스템

### Intersect() 메서드

```cpp
_bool CCollider::Intersect(CCollider* pTargetCollider)
{
    // 충돌 활성화 상태 확인
    if (false == *m_bCollisionActivated && 
        false == pTargetCollider->Get_Other_Collision_Activated() || 
        !m_bColliderActive)
    {
        m_bIsCollision = false;
        pTargetCollider->Set_Collision(false);
        return false;
    }
    
    // Bounding의 Intersect 메서드 호출
    m_bIsCollision = m_pBounding->Intersect(
        pTargetCollider->m_eColliderType,  // 대상 Collider 타입
        pTargetCollider->m_pBounding        // 대상 Bounding 포인터
    );
    
    pTargetCollider->Set_Collision(m_bIsCollision);
    
    return m_bIsCollision;
}
```

**동작 과정:**
1. **활성화 상태 확인**: 양쪽 Collider가 모두 활성화되어 있고, 충돌 활성화 플래그가 켜져 있는지 확인합니다.
2. **Bounding 충돌 검사**: 내부 Bounding 객체의 `Intersect()` 메서드를 호출하여 실제 충돌 여부를 판단합니다.
3. **결과 반환**: 충돌 상태를 양쪽 Collider에 설정하고 결과를 반환합니다.

**최적화:**
- 비활성화된 Collider는 즉시 `false`를 반환하여 불필요한 계산을 방지합니다.
- Bounding 타입에 따라 최적화된 충돌 검사 알고리즘이 사용됩니다 (AABB vs AABB는 가장 빠름).

---

## 충돌 상태 관리 (ENTER/STAY/EXIT)

### Collided_With() - 충돌 기록

```cpp
void CCollider::Collided_With(CCollider* pOther)
{
    m_curCollisions.insert(pOther);
}
```

**역할:**
- `CCollision_Manager`가 충돌 검사 후 호출하는 메서드입니다.
- 현재 프레임에 충돌한 Collider를 `m_curCollisions` Set에 추가합니다.
- **이 시점에서는 아직 GameObject에 신호를 보내지 않습니다.**

### Process_Collisions() - 충돌 상태 판단 및 이벤트 전달

```cpp
void CCollider::Process_Collisions()
{
    // Enter or Stay 처리
    for (auto* pOther : m_curCollisions)  // 현재 프레임에 충돌한 Collider들
    {
        if (m_prevCollisions.find(pOther) != m_prevCollisions.end())
        {
            // 이전 프레임에도 충돌했음 → STAY
            if (nullptr == m_pOwnerGameObject)
                return;
            
            m_pOwnerGameObject->Collided_With(pOther, STAY);
        }
        else
        {
            // 이전 프레임에는 충돌하지 않았음 → ENTER
            if (nullptr == m_pOwnerGameObject)
                return;
            
            m_pOwnerGameObject->Collided_With(pOther, ENTER);
        }
    }

    // Exit 처리
    for (auto* pOther : m_prevCollisions)  // 이전 프레임에 충돌했던 Collider들
    {
        if (m_curCollisions.find(pOther) == m_curCollisions.end())
        {
            // 이전 프레임에는 충돌했지만 현재는 안 함 → EXIT
            m_pOwnerGameObject->Collided_With(pOther, EXIT);
        }
    }

    // 다음 프레임을 위해 상태 업데이트
    m_prevCollisions = std::move(m_curCollisions);
    m_curCollisions.clear();
}
```

**충돌 상태 판단 로직:**

| 이전 프레임 | 현재 프레임 | 상태 | 의미 |
|-----------|-----------|------|------|
| 충돌 없음 | 충돌 있음 | **ENTER** | 충돌이 시작됨 |
| 충돌 있음 | 충돌 있음 | **STAY** | 충돌이 지속됨 |
| 충돌 있음 | 충돌 없음 | **EXIT** | 충돌이 종료됨 |

**최적화 포인트:**
- `unordered_set`을 사용하여 O(1) 시간에 충돌 상태를 확인합니다.
- `std::move`를 사용하여 Set 복사를 최소화합니다.
- 충돌하지 않은 Collider는 `Process_Collisions()`가 빠르게 종료됩니다.

---

## Collision_Manager와의 연동

### 전체 흐름

```
1. Collision_Manager::Update()
   ├── 모든 Collider의 Update() 호출 (Bounding 업데이트)
   ├── Collider 간 Intersect() 호출 (충돌 검사)
   ├── 충돌한 경우 Collided_With() 호출 (Set에 추가)
   └── 모든 Collider의 Process_Collisions() 호출 (상태 판단 및 이벤트 전달)
```

### Collision_Manager의 역할

```cpp
// Collision_Manager::Update() 내부
void CCollision_Manager::Update()
{
    // 1. 모든 Collider의 Bounding 업데이트
    for (auto& group : colliderGroups)
    {
        for (auto* collider : group.second)
        {
            if (collider)
                collider->Update();
        }
    }
    
    // 2. 서로 다른 그룹 간 충돌 검사
    for (auto iterA = colliderGroups.begin(); iterA != colliderGroups.end(); ++iterA)
    {
        auto& groupA = iterA->second;
        auto iterB = iterA;
        ++iterB;
        
        for (; iterB != colliderGroups.end(); ++iterB)
        {
            auto& groupB = iterB->second;
            
            for (auto* pColliderA : groupA)
            {
                if (!pColliderA || pColliderA->Is_MouseCollider()) continue;
                
                for (auto* pColliderB : groupB)
                {
                    if (!pColliderB || pColliderB->Is_MouseCollider()) continue;
                    
                    if (pColliderA->Intersect(pColliderB))
                    {
                        // 충돌한 경우 Set에 추가만 함
                        pColliderA->Collided_With(pColliderB);
                        pColliderB->Collided_With(pColliderA);
                    }
                }
            }
        }
    }
    
    // 3. 충돌 상태 처리 및 GameObject에 신호 전달
    for (auto& group : colliderGroups)
    {
        for (auto* collider : group.second)
        {
            if (collider)
                collider->Process_Collisions();  // ENTER/STAY/EXIT 판단 및 전달
        }
    }
}
```

**책임 분리:**
- **Collision_Manager**: 충돌 검사 스케줄링, 그룹 관리, 물리 처리
- **Collider**: Bounding 업데이트, 충돌 상태 관리, GameObject에 이벤트 전달
- **GameObject**: 충돌 이벤트를 받아 게임 로직 처리

---

## 주요 속성 및 설정

### COLLIDER_ROLE (충돌체 역할)

```cpp
enum COLLIDER_ROLE { SMALL, BIG, ETC };
```

- **SMALL**: 작은 동적 오브젝트 (플레이어, 몬스터 등)
  - SMALL vs SMALL 충돌 시 물리 처리(침투 해결 및 슬라이딩) 적용
- **BIG**: 큰 정적 오브젝트 (환경 구조물 등)
  - 물리 처리 대상에서 제외
- **ETC**: 기타 특수 용도

**사용 예시:**
```cpp
// 플레이어 Collider 설정
pPlayerCollider->Set_ColliderRole(CCollider::SMALL);

// 벽 Collider 설정
pWallCollider->Set_ColliderRole(CCollider::BIG);
```

### Collider 활성화/비활성화

```cpp
void Set_ColliderActive(_bool ColliderActive);
_bool Is_ColliderActive() const;
```

**활용 사례:**
- 플레이어가 죽었을 때: `Set_ColliderActive(false)`로 충돌 감지 비활성화
- 오브젝트가 비활성화될 때: Collider도 함께 비활성화하여 불필요한 충돌 검사 방지
- 특정 상태에서만 충돌이 필요한 경우: 상태에 따라 동적으로 활성/비활성

### Collider Tag

```cpp
void Set_ColliderTag(const _wstring& strColliderTag);
const _wstring& Get_ColliderTag();
```

**용도:**
- 충돌 이벤트 처리 시 Collider를 식별
- 예: `TEXT("Player")`, `TEXT("Monster")`, `TEXT("Arrow")` 등

### Mouse Collider (마우스 피킹용)

```cpp
void Set_MouseCollider(_bool ColliderForMouse);
_bool Is_MouseCollider() const;
```

**특징:**
- 마우스 피킹 전용 Collider는 일반 충돌 검사에서 제외됩니다.
- `Collision_Manager::Update()`에서 `Is_MouseCollider()`가 `true`인 경우 스킵합니다.

### Same Group Collision (같은 그룹 내 충돌)

```cpp
void Set_AllowSameGroupCollision(_bool bAllowCollision);
_bool Get_AllowSameGroupCollision() const;
```

**용도:**
- 기본적으로 Collision_Manager는 서로 다른 그룹 간 충돌만 검사합니다.
- 같은 그룹 내 충돌이 필요한 경우(예: 플레이어들 간 충돌) 이 플래그를 활성화합니다.

---

## 디버그 렌더링

### 구현

```cpp
#ifdef _DEBUG
HRESULT CCollider::Render()
{
    if (!m_bColliderActive)
        return S_OK;

    // DirectXTK의 BasicEffect 사용
    m_pEffect->SetWorld(XMMatrixIdentity());
    m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
    m_pContext->IASetInputLayout(m_pInputLayout);
    
    m_pEffect->Apply(m_pContext);

    // 충돌 상태에 따라 색상 변경
    // 충돌 중: 빨간색 (1, 0, 0, 1)
    // 충돌 없음: 초록색 (0, 1, 0, 1)
    m_pBounding->Render(
        m_pBatch, 
        true == m_bIsCollision ? 
            XMVectorSet(1.f, 0.f, 0.f, 1.f) : 
            XMVectorSet(0.f, 1.f, 0.f, 1.f)
    );

    return S_OK;
}
#endif
```

**기능:**
- 개발 중에는 모든 Collider의 Bounding 영역을 시각화합니다.
- 충돌 중인 Collider는 빨간색, 충돌하지 않은 Collider는 초록색으로 표시됩니다.
- 릴리즈 빌드에서는 컴파일되지 않아 성능 영향이 없습니다.

---

## 실제 사용 예시

### 1. 플레이어 Collider 생성

```cpp
// Player.cpp - Ready_Components()
HRESULT CPlayer::Ready_Components()
{
    // ... 다른 컴포넌트들 ...
    
    // Collider 컴포넌트 추가
    CBounding::BOUNDING_DESC BoundingDesc{};
    BoundingDesc.vCenter = _float3(0.f, 0.5f, 0.f);  // 중심점 (높이 0.5)
    BoundingDesc.fRadius = 0.5f;                     // 반지름 0.5
    BoundingDesc.CombinedWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    BoundingDesc.pGameObject = this;
    BoundingDesc.pCollisionActivated = &m_bCollisionActivated;
    
    if (FAILED(Add_Component(
        LEVEL_STATIC,
        TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider"),
        &BoundingDesc)))
        return E_FAIL;
    
    // Collider 설정
    m_pColliderCom = static_cast<CCollider*>(
        Find_Component(TEXT("Com_Collider")));
    m_pColliderCom->Set_ColliderRole(CCollider::SMALL);
    m_pColliderCom->Set_ColliderTag(TEXT("Player"));
    
    return S_OK;
}
```

### 2. 충돌 이벤트 처리 (GameObject)

```cpp
// Player.cpp - Collided_With()
void CPlayer::Collided_With(CCollider* pOther, CCollider::COLLISION_STATE eCollisionState)
{
    switch (eCollisionState)
    {
    case CCollider::ENTER:
        // 충돌 시작
        if (pOther->Get_ColliderTag() == TEXT("Monster"))
        {
            // 몬스터와 충돌 시작
            m_pPlayerFSM->Collision_Enter(pOther);
        }
        else if (pOther->Get_ColliderTag() == TEXT("Item"))
        {
            // 아이템 획득
            Pickup_Item(pOther->Get_OwnerObject());
        }
        break;
        
    case CCollider::STAY:
        // 충돌 지속
        m_pPlayerFSM->Collision_Stay(pOther);
        break;
        
    case CCollider::EXIT:
        // 충돌 종료
        m_pPlayerFSM->Collision_Exit(pOther);
        break;
    }
}
```

### 3. FSM State에서의 충돌 처리

```cpp
// State_Player_Idle.cpp
void CState_Player_Idle::Collision_Enter(CCollider* pOther)
{
    if (pOther->Get_ColliderTag() == TEXT("Monster_Attack"))
    {
        // 몬스터 공격에 맞음
        m_pPlayer->Take_Damage(10);
        m_pPlayer->Get_PlayerFSM()->Change_State(CPlayer::STATE_HIT);
    }
}

void CState_Player_Roll::Collision_Enter(CCollider* pOther)
{
    // 구르기 중에는 공격 무시
    // (충돌 이벤트를 받지만 처리하지 않음)
}
```

**특징:**
- 각 State는 자신이 관심 있는 충돌만 처리할 수 있습니다.
- State 변경 시 자동으로 새로운 State가 충돌 이벤트를 받습니다.
- 현재 활성화된 State에만 충돌 신호가 전달됩니다.

### 4. 다중 Collider (PartObject)

```cpp
// Player.cpp - PartObject 준비
HRESULT CPlayer::Ready_PartObjects()
{
    // Body Part에 Collider 추가
    CBody_Player::BODY_PLAYER_DESC BodyDesc{};
    BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    
    Add_PartObject(..., TEXT("Part_Body"), &BodyDesc);
    
    // Body PartObject 내부에서 Collider 추가
    // Body_Player.cpp
    CBounding::BOUNDING_DESC BoundingDesc{};
    BoundingDesc.vCenter = _float3(0.f, 0.5f, 0.f);
    BoundingDesc.fRadius = 0.5f;
    BoundingDesc.CombinedWorldMatrix = /* Body의 World Matrix */;
    
    Add_Component(..., TEXT("Com_Collider_Body"), &BoundingDesc);
    
    // Weapon Part에 별도의 Collider 추가 (공격 범위)
    // Weapon_Player.cpp
    BoundingDesc.vCenter = _float3(0.f, 0.f, 1.f);  // 무기 앞쪽
    BoundingDesc.fRadius = 0.3f;
    
    Add_Component(..., TEXT("Com_Collider_Weapon"), &BoundingDesc);
    m_pWeaponCollider->Set_ColliderTag(TEXT("Player_Attack"));
}
```

**장점:**
- 하나의 GameObject가 여러 Collider를 가질 수 있습니다.
- Body Collider와 Weapon Collider를 독립적으로 관리할 수 있습니다.
- 각 PartObject는 자신만의 Collider를 가질 수 있습니다.

---

## 성능 최적화

### 1. 활성화 상태 확인

```cpp
_bool CCollider::Intersect(CCollider* pTargetCollider)
{
    // 조기 반환으로 불필요한 계산 방지
    if (!m_bColliderActive || 
        false == *m_bCollisionActivated)
        return false;
    
    // ... 실제 충돌 검사 ...
}
```

### 2. Set 기반 충돌 상태 관리

```cpp
// unordered_set을 사용하여 O(1) 시간에 충돌 상태 확인
unordered_set<CCollider*> m_prevCollisions;  // 이전 프레임
unordered_set<CCollider*> m_curCollisions;   // 현재 프레임
```

**효과:**
- 충돌하지 않은 Collider는 `Process_Collisions()`가 즉시 종료됩니다.
- Set 비교는 O(n) 시간에 수행되며, n은 충돌한 Collider의 개수입니다.
- 일반적으로 한 프레임에 충돌하는 Collider는 전체의 일부이므로 매우 효율적입니다.

### 3. Collision_Manager의 그룹화

```cpp
// Collision_Manager는 Collider를 그룹별로 관리
unordered_map<wstring, vector<CCollider*>> colliderGroups;

// 서로 다른 그룹 간에만 충돌 검사
// → 같은 그룹 내 충돌 검사는 기본적으로 생략
```

**효과:**
- 플레이어와 몬스터는 서로 다른 그룹이므로 충돌 검사가 수행됩니다.
- 같은 그룹 내 충돌이 불필요한 경우(예: 몬스터들 간 충돌) 검사를 생략할 수 있습니다.
- 충돌 검사 횟수가 그룹 수에 비례하여 감소합니다.

### 4. Bounding 타입별 최적화

| Bounding 타입 | 충돌 검사 시간 복잡도 | 특징 |
|--------------|-------------------|------|
| AABB vs AABB | O(1) | 가장 빠름, 단순 좌표 비교 |
| Sphere vs Sphere | O(1) | 매우 빠름, 거리 계산만 필요 |
| OBB vs OBB | O(k), k≈15 | 상대적으로 느림, SAT 알고리즘 |

**프로젝트에서의 사용:**
- 대부분의 동적 오브젝트는 Sphere Collider를 사용합니다.
- Sphere의 계산 효율성과 회전 독립성이 게임플레이에 적합합니다.

---

## 컴포넌트 시스템의 이점

### 1. 책임 분리

- **Collider**: 충돌 감지 및 상태 관리만 담당
- **Collision_Manager**: 충돌 검사 스케줄링 및 그룹 관리
- **GameObject**: 충돌 이벤트를 받아 게임 로직 처리

### 2. 재사용성

```cpp
// 한 번 작성된 Collider 컴포넌트를 모든 오브젝트에 사용
CCollider* pCollider = CCollider::Create(pDevice, pContext, TYPE_SPHERE);

// Player에 추가
pPlayer->Add_Component(..., TEXT("Com_Collider"), &pCollider);

// Monster에 추가
pMonster->Add_Component(..., TEXT("Com_Collider"), &pCollider);

// Item에 추가
pItem->Add_Component(..., TEXT("Com_Collider"), &pCollider);
```

### 3. 런타임 유연성

- Collider를 런타임에 활성/비활성화할 수 있습니다.
- 다중 Collider를 동적으로 추가/제거할 수 있습니다.
- Collider 설정을 런타임에 변경할 수 있습니다.

### 4. 확장성

- 새로운 Bounding 타입 추가 시 Collider 코드 수정이 최소화됩니다.
- Collider의 기능 확장이 다른 컴포넌트에 영향을 주지 않습니다.

---

## 결론

`CCollider` 컴포넌트는 다음과 같은 핵심 기능을 제공합니다:

1. **Bounding 기반 충돌 감지**: AABB, OBB, Sphere 세 가지 타입 지원
2. **충돌 상태 관리**: ENTER/STAY/EXIT 상태를 추적하고 GameObject에 전달
3. **Collision_Manager와의 협력**: 중앙 집중식 충돌 검사로 효율성 향상
4. **런타임 제어**: 활성/비활성, 역할 설정, 태그 관리 등
5. **디버그 지원**: 개발 중 Bounding 영역 시각화

이러한 설계로 **충돌 감지와 게임 로직의 완전한 분리**를 달성하여, 코드의 유지보수성과 확장성을 크게 향상시켰습니다. 컴포넌트 기반 아키텍처의 장점을 충분히 활용한 시스템입니다.

