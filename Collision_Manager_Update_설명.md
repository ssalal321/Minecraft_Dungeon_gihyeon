# Collision_Manager::Update() 상세 설명

## 개요
`Collision_Manager::Update()`는 매 프레임마다 모든 Collider 간의 충돌을 검사하고, 충돌이 발생한 경우에만 해당 오브젝트의 State에 신호를 전달하는 최적화된 충돌 처리 시스템입니다.

## 전체 흐름

### 1단계: Collider Update
```cpp
// 1. Collider Update
for (auto& group : colliderGroups)
{
    for (auto* collider : group.second)
    {
        if (collider)
            collider->Update();
    }
}
```
- **목적**: 모든 Collider의 Bounding 정보를 현재 프레임의 Transform 정보로 업데이트
- 모든 Collider에 대해 실행되며, Bounding의 위치/회전을 갱신합니다.

---

### 2단계: 서로 다른 그룹 간 충돌 검사
```cpp
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
            if (!pColliderA || pColliderA->Get_MouseCollider()) continue;

            for (auto* pColliderB : groupB)
            {
                if (!pColliderB || pColliderB->Get_MouseCollider()) continue;

                if (pColliderA->Intersect(pColliderB))  // 실제 충돌 검사
                {
                    pColliderA->Collided_With(pColliderB);  // Set에 추가
                    pColliderB->Collided_With(pColliderA);
                    
                    // 물리 처리 (SMALL-SMALL 충돌만)
                    if (pColliderA->Get_Role() == CCollider::SMALL && 
                        pColliderB->Get_Role() == CCollider::SMALL && 
                        pColliderA < pColliderB)
                    {
                        Resolve_Penetration_And_Slide(pColliderA, pColliderB, 9.f);
                    }
                }
            }
        }
    }
}
```

**핵심 최적화 포인트 1: 충돌 발생 시에만 Set에 추가**
- `Intersect()`가 `true`를 반환한 경우에만 `Collided_With()`를 호출합니다.
- `Collided_With()`는 단순히 `m_currCollisions` Set에 충돌한 Collider를 추가하는 작업만 수행합니다.
- **이 시점에서는 아직 GameObject나 State에 신호를 보내지 않습니다.**

**최적화 효과**:
- 충돌하지 않은 Collider에 대해서는 추가 처리 오버헤드가 없습니다.
- 충돌 검사 결과만 Set에 기록하고, 실제 게임 로직 처리는 나중에 수행합니다.

---

### 3단계: 같은 그룹 내 충돌 검사 (옵션)
```cpp
// 3. 같은 그룹 내 충돌 검사 (AllowInGroupCollision 활성화된 경우만)
for (auto& group : colliderGroups)
{
    auto& colliders = group.second;

    for (size_t i = 0; i < colliders.size(); ++i)
    {
        CCollider* pColliderA = colliders[i];
        if (!pColliderA || !pColliderA->Get_AllowSameGroupCollision() || 
            pColliderA->Get_MouseCollider()) continue;

        for (size_t j = i + 1; j < colliders.size(); ++j)
        {
            CCollider* pColliderB = colliders[j];
            if (!pColliderB || !pColliderB->Get_AllowSameGroupCollision() || 
                pColliderB->Get_MouseCollider()) continue;

            if (pColliderA->Intersect(pColliderB))
            {
                pColliderA->Collided_With(pColliderB);
                pColliderB->Collided_With(pColliderA);
                // 물리 처리...
            }
        }
    }
}
```

**최적화 포인트**:
- `AllowSameGroupCollision`이 활성화된 Collider에 대해서만 그룹 내 충돌 검사를 수행합니다.
- 예: 플레이어들 간의 충돌이 필요한 경우에만 사용

---

### 4단계: 충돌 상태 처리 및 State 신호 전달 ⭐ **핵심 최적화 단계**
```cpp
// 4. 충돌 상태 처리 (Enter / Stay / Exit 처리)
for (auto& group : colliderGroups)
{
    for (auto* collider : group.second)
    {
        if (collider)
            collider->Process_Collisions();
    }
}
```

이 단계가 가장 중요한 최적화 포인트입니다. `CCollider::Process_Collisions()` 내부를 살펴보면:

```cpp
void CCollider::Process_Collisions()
{
    // Enter or Stay 처리
    for (auto* pOther : m_currCollisions)  // 현재 프레임에 충돌한 Collider들
    {
        if (m_prevCollisions.find(pOther) != m_prevCollisions.end())
        {
            // 이전 프레임에도 충돌했음 → STAY
            m_pOwnerGameObject->Collided_With(pOther, STAY);
        }
        else
        {
            // 이전 프레임에는 충돌하지 않았음 → ENTER
            m_pOwnerGameObject->Collided_With(pOther, ENTER);
        }
    }

    // Exit 처리
    for (auto* pOther : m_prevCollisions)  // 이전 프레임에 충돌했던 Collider들
    {
        if (m_currCollisions.find(pOther) == m_currCollisions.end())
        {
            // 이전 프레임에는 충돌했지만 현재는 안 함 → EXIT
            m_pOwnerGameObject->Collided_With(pOther, EXIT);
        }
    }

    // 다음 프레임을 위해 상태 업데이트
    m_prevCollisions = std::move(m_currCollisions);
    m_currCollisions.clear();
}
```

**핵심 최적화 포인트 2: 충돌한 Collider에 대해서만 GameObject에 신호 전달**
- 2단계와 3단계에서 `m_currCollisions` Set에 추가된 Collider들에 대해서만 처리합니다.
- 충돌하지 않은 Collider는 `m_currCollisions`가 비어있으므로, `Process_Collisions()`가 빠르게 종료됩니다.

**핵심 최적화 포인트 3: 현재 State에만 신호 전달**

GameObject의 `Collided_With()` 구현 예시 (Player.cpp):
```cpp
void CPlayer::Collided_With(CCollider* pOther, CCollider::COLLISION_STATE eCollisionState)
{
    switch (eCollisionState)
    {
    case CCollider::ENTER:
        m_pPlayerFSM->Collision_Enter(pOther);  // FSM을 통해 현재 State에만 전달
        break;

    case CCollider::STAY:
        m_pPlayerFSM->Collision_Stay(pOther);
        break;

    case CCollider::EXIT:
        m_pPlayerFSM->Collision_Exit(pOther);
        break;
    }
}
```

FSM의 `Collision_Enter()` 구현 (FSM.h):
```cpp
void Collision_Enter(CCollider* _other)
{
    if (m_curState != nullptr)
        m_curState->Collision_Enter(_other);  // 현재 State의 메서드만 호출
}
```

**최적화 효과**:
- 오브젝트가 IDLE 상태일 때는 `CState_Player_Idle::Collision_Enter()`만 호출됩니다.
- ATTACK 상태일 때는 `CState_Player_Attack::Collision_Enter()`만 호출됩니다.
- **모든 State를 순회하면서 Update를 호출하는 것이 아닙니다.**
- **현재 활성화된 State에만 충돌 신호가 전달됩니다.**

---

## 최적화 요약

### 1. **충돌 검사 결과를 Set에 기록만 함** (2-3단계)
- 실제 충돌이 발생한 경우에만 `m_currCollisions` Set에 추가
- 충돌하지 않은 Collider는 추가 처리 없음

### 2. **충돌한 Collider에 대해서만 상태 처리** (4단계)
- `Process_Collisions()`는 `m_currCollisions`가 비어있으면 즉시 종료
- Enter/Stay/Exit 판단은 Set 비교로 빠르게 수행

### 3. **현재 State에만 신호 전달** (GameObject → FSM → State)
- GameObject는 FSM에 신호 전달
- FSM은 현재 활성화된 State에만 메서드 호출
- **모든 State를 순회하지 않고, 현재 State만 업데이트**

### 4. **State별 선택적 충돌 처리**
- 각 State는 자신이 관심 있는 충돌만 처리하면 됩니다.
- 예: `CState_Player_Idle`은 충돌 시 데미지를 받지만, `CState_Player_Roll`은 무시할 수 있습니다.

---

## 성능 비교

### 비최적화 방식 (만약 이렇게 구현했다면)
```cpp
// 모든 GameObject의 모든 State를 순회
for (auto* gameObject : allGameObjects)
{
    for (auto* state : gameObject->allStates)
    {
        state->Check_Collisions();  // 모든 State가 매 프레임 충돌 검사
    }
}
```

### 현재 최적화 방식
```cpp
// 1. 충돌 검사는 Collision_Manager가 한 번만 수행
// 2. 충돌한 Collider만 Set에 기록
// 3. 충돌한 Collider에 대해서만 Process_Collisions() 호출
// 4. 현재 활성화된 State에만 신호 전달
```

**성능 향상**:
- 충돌하지 않은 오브젝트: 거의 0의 오버헤드
- 충돌한 오브젝트: 현재 State의 메서드만 호출
- State 변경 시 자동으로 새로운 State가 충돌 신호를 받음

---

## 결론

이 시스템의 핵심 최적화는 **"충돌이 발생한 경우에만, 현재 활성화된 State에만 신호를 전달"**하는 것입니다.

1. **충돌 검사 단계**: 실제 충돌이 발생한 경우에만 Set에 기록
2. **상태 처리 단계**: Set에 기록된 Collider에 대해서만 Enter/Stay/Exit 판단
3. **신호 전달 단계**: 현재 활성화된 State에만 메서드 호출

이를 통해 불필요한 State 순회와 함수 호출을 최소화하여 성능을 향상시켰습니다.
