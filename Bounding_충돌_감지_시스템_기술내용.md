# Bounding 충돌 감지 시스템 기술소개서

## 개요

본 프로젝트는 **DirectX Math 라이브러리의 Bounding Volume**을 활용하여 세 가지 타입의 충돌 감지 시스템을 구현하였습니다. AABB(Axis-Aligned Bounding Box), OBB(Oriented Bounding Box), Sphere 세 가지 타입을 모두 지원하며, 각 타입 간 상호 충돌 검사가 가능하도록 설계되었습니다.

## 아키텍처

### 클래스 구조

```
CBounding (추상 기본 클래스)
├── CBounding_AABB
├── CBounding_OBB
└── CBounding_Sphere
```

모든 Bounding 타입은 `CBounding` 추상 클래스를 상속받아 일관된 인터페이스를 제공합니다.

### 핵심 인터페이스

```cpp
class CBounding abstract
{
public:
    virtual void Update(_fmatrix WorldMatrix) = 0;  // 월드 변환 업데이트
    virtual _bool Intersect(COLLIDER_TYPE eColliderType, 
                           CBounding* pTargetBounding, 
                           void* pRayArg = nullptr) = 0;  // 충돌 검사
};
```

---

## 1. AABB (Axis-Aligned Bounding Box) - 축 정렬 경계 박스

### 개념

AABB는 **축에 정렬된 직육면체**로, 월드 좌표계의 X, Y, Z 축과 평행한 경계 박스입니다. 회전하지 않는 박스 형태의 충돌 영역을 표현합니다.

### 특징

- **정렬 방식**: 항상 월드 좌표계의 축과 평행
- **변환 최적화**: 회전 변환을 제거하고 스케일만 적용하여 계산 비용 최소화
- **적용 사례**: 환경 오브젝트, 정적 구조물 등 회전이 없는 오브젝트

### 구현 상세

#### 초기화
```cpp
HRESULT CBounding_AABB::Initialize(const BOUNDING_DESC* pArg)
{
    const BOUNDING_AABB_DESC* pDesc = static_cast<const BOUNDING_AABB_DESC*>(pArg);
    
    // 중심점과 반경(Extents)으로 로컬 AABB 생성
    m_pLocalDesc = new BoundingBox(pDesc->vCenter, pDesc->vExtents);
    m_pDesc = new BoundingBox(*m_pLocalDesc);  // 월드 변환된 AABB
    
    return S_OK;
}
```

#### 월드 변환 (Update)
```cpp
void CBounding_AABB::Update(_fmatrix WorldMatrix)
{
    _matrix TransformMatrix = WorldMatrix;
    
    // 회전 성분 제거, 스케일만 유지
    TransformMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(TransformMatrix.r[0]);
    TransformMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(TransformMatrix.r[1]);
    TransformMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(TransformMatrix.r[2]);
    
    // AABB는 회전하지 않으므로 스케일과 위치만 변환
    m_pLocalDesc->Transform(*m_pDesc, TransformMatrix);
}
```

**핵심 최적화**: AABB는 회전을 지원하지 않으므로, 월드 매트릭스에서 회전 성분을 제거하고 스케일 벡터의 길이만 추출하여 적용합니다. 이를 통해 **불필요한 행렬 연산을 제거**합니다.

#### 충돌 검사
```cpp
_bool CBounding_AABB::Intersect(COLLIDER_TYPE eColliderType, 
                                CBounding* pTargetBounding, void* pRayArg)
{
    void* pTargetDesc = pTargetBounding->Get_Desc();
    _bool isColl = false;
    
    switch (eColliderType)
    {
    case COLLIDER_TYPE::TYPE_AABB:
        // AABB vs AABB: 가장 빠른 충돌 검사
        isColl = m_pDesc->Intersects(*static_cast<BoundingBox*>(pTargetDesc));
        break;
    case COLLIDER_TYPE::TYPE_OBB:
        // AABB vs OBB
        isColl = m_pDesc->Intersects(*static_cast<BoundingOrientedBox*>(pTargetDesc));
        break;
    case COLLIDER_TYPE::TYPE_SPHERE:
        // AABB vs Sphere
        isColl = m_pDesc->Intersects(*static_cast<BoundingSphere*>(pTargetDesc));
        break;
    }
    
    return isColl;
}
```

**성능**: AABB vs AABB 충돌 검사는 **단순한 좌표 범위 비교**로 O(1) 시간에 수행되어 가장 빠릅니다.

---

## 2. OBB (Oriented Bounding Box) - 방향성 경계 박스

### 개념

OBB는 **임의의 방향으로 회전된 직육면체**입니다. 로컬 좌표계의 회전 정보를 포함하여 오브젝트의 실제 방향을 반영합니다.

### 특징

- **회전 지원**: 오브젝트의 회전을 정확히 반영
- **정밀도**: AABB보다 오브젝트의 실제 형태에 더 근접한 경계 체적
- **적용 사례**: 회전하는 오브젝트, 도어, 상자 등

### 구현 상세

#### 초기화
```cpp
HRESULT CBounding_OBB::Initialize(const BOUNDING_DESC* pArg)
{
    const BOUNDING_OBB_DESC* pDesc = static_cast<const BOUNDING_OBB_DESC*>(pArg);
    
    // 오일러 각도를 쿼터니언으로 변환
    _float4 vQuaternion = {};
    XMStoreFloat4(&vQuaternion, 
        XMQuaternionRotationRollPitchYaw(
            pDesc->vRotation.x,  // Pitch
            pDesc->vRotation.y,  // Yaw
            pDesc->vRotation.z   // Roll
        ));
    
    // 중심점, 반경, 회전 쿼터니언으로 OBB 생성
    m_pLocalDesc = new BoundingOrientedBox(pDesc->vCenter, pDesc->vExtents, vQuaternion);
    m_pDesc = new BoundingOrientedBox(*m_pLocalDesc);
    
    return S_OK;
}
```

#### 월드 변환 (Update)
```cpp
void CBounding_OBB::Update(_fmatrix WorldMatrix)
{
    // OBB는 회전을 지원하므로 전체 월드 매트릭스 적용
    m_pLocalDesc->Transform(*m_pDesc, WorldMatrix);
}
```

**차이점**: AABB와 달리 OBB는 **전체 월드 매트릭스(위치, 회전, 스케일)**를 그대로 적용합니다.

#### 충돌 검사
```cpp
_bool CBounding_OBB::Intersect(COLLIDER_TYPE eColliderType, 
                               CBounding* pTargetBounding, void* pRayArg)
{
    void* pTargetDesc = pTargetBounding->Get_Desc();
    _bool isColl = false;
    
    switch (eColliderType)
    {
    case COLLIDER_TYPE::TYPE_AABB:
        isColl = m_pDesc->Intersects(*static_cast<BoundingBox*>(pTargetDesc));
        break;
    case COLLIDER_TYPE::TYPE_OBB:
        // OBB vs OBB: SAT(Separating Axis Theorem) 알고리즘 사용
        isColl = m_pDesc->Intersects(*static_cast<BoundingOrientedBox*>(pTargetDesc));
        break;
    case COLLIDER_TYPE::TYPE_SPHERE:
        isColl = m_pDesc->Intersects(*static_cast<BoundingSphere*>(pTargetDesc));
        break;
    }
    
    return isColl;
}
```

**알고리즘**: OBB vs OBB 충돌 검사는 DirectX Math의 내부 구현을 사용하며, 일반적으로 **SAT(Separating Axis Theorem)** 알고리즘을 기반으로 합니다.

---

## 3. Sphere - 경계 구

### 개념

Sphere는 **중심점과 반지름**으로 정의되는 구 형태의 경계 체적입니다. 가장 단순하면서도 효율적인 충돌 감지 타입입니다.

### 특징

- **계산 효율성**: 거리 기반 계산으로 매우 빠름
- **회전 독립성**: 회전해도 형태가 변하지 않음
- **적용 사례**: 플레이어, 몬스터, 투사체 등 대부분의 동적 오브젝트
- **추가 기능**: Ray 충돌 검사 지원 (마우스 피킹 등)

### 구현 상세

#### 초기화
```cpp
HRESULT CBounding_Sphere::Initialize(const BOUNDING_DESC* pArg)
{
    const BOUNDING_SPHERE_DESC* pDesc = static_cast<const BOUNDING_SPHERE_DESC*>(pArg);
    
    // 중심점과 반지름으로 구 생성
    m_pLocalDesc = new BoundingSphere(pDesc->vCenter, pDesc->fRadius);
    m_pDesc = new BoundingSphere(*m_pLocalDesc);
    
    return S_OK;
}
```

#### 월드 변환 (Update)5
```cpp
void CBounding_Sphere::Update(_fmatrix WorldMatrix)
{
    // 구는 회전에 독립적이지만, 스케일은 반지름에 영향을 줌
    m_pLocalDesc->Transform(*m_pDesc, WorldMatrix);
}
```

**특징**: 구는 회전해도 형태가 변하지 않지만, 월드 매트릭스의 스케일은 반지름에 영향을 줍니다.

#### 충돌 검사
```cpp
_bool CBounding_Sphere::Intersect(COLLIDER_TYPE eColliderType, 
                                  CBounding* pTargetBounding, void* pArg)
{
    void* pTargetDesc = nullptr;
    if (pTargetBounding)
        pTargetDesc = pTargetBounding->Get_Desc();
    
    _bool isColl = false;
    
    switch (eColliderType)
    {
    case COLLIDER_TYPE::TYPE_AABB:
        isColl = m_pDesc->Intersects(*static_cast<BoundingBox*>(pTargetDesc));
        break;
    case COLLIDER_TYPE::TYPE_OBB:
        isColl = m_pDesc->Intersects(*static_cast<BoundingOrientedBox*>(pTargetDesc));
        break;
    case COLLIDER_TYPE::TYPE_SPHERE:
        // Sphere vs Sphere: 중심점 간 거리와 반지름 합 비교
        // distance(C1, C2) <= (R1 + R2) 이면 충돌
        isColl = m_pDesc->Intersects(*static_cast<BoundingSphere*>(pTargetDesc));
        break;
        
    case COLLIDER_TYPE::TYPE_RAY:
        {
            // Ray 충돌 검사 (마우스 피킹 등에 사용)
            RayDesc* pRayDesc = static_cast<RayDesc*>(pArg);
            if (nullptr == pRayDesc)
                return false;
            
            _fvector vMousePos = XMLoadFloat3(&pRayDesc->MousePos);
            _fvector vMouseRay = XMLoadFloat3(&pRayDesc->MouseRay);
            
            if (XMVectorGetX(XMVector3Length(vMouseRay)) == 0.f)
                return false;
            
            _float dst = {};
            if (nullptr == pRayDesc->fDist)
                pRayDesc->fDist = &dst;
            
            // Ray와 구의 충돌 검사 및 거리 반환
            isColl = m_pDesc->Intersects(vMousePos, vMouseRay, *pRayDesc->fDist);
        }
        break;
    }
    
    return isColl;
}
```

**Sphere vs Sphere 충돌 검사 알고리즘**:
```
두 구의 중심점 간 거리: d = |C1 - C2|
두 구의 반지름 합: r = R1 + R2
충돌 조건: d <= r
```

이 계산은 단순한 벡터 뺄셈과 길이 계산이므로 **매우 빠릅니다**.

**Ray 충돌 검사**: Sphere는 추가로 **Ray(광선)와의 충돌 검사**를 지원합니다. 이는 마우스 피킹, 시야 확인, 발사체 충돌 등에 활용됩니다.

---

## 상호 호환성 및 충돌 검사 매트릭스

세 가지 Bounding 타입은 모두 서로 간의 충돌 검사를 지원합니다:

|           | vs AABB | vs OBB | vs Sphere | vs Ray |
|-----------|---------|--------|-----------|--------|
| **AABB**  | ✅      | ✅     | ✅        | ❌     |
| **OBB**   | ✅      | ✅     | ✅        | ❌     |
| **Sphere**| ✅      | ✅     | ✅        | ✅     |

**설계 원칙**: `Intersect()` 메서드는 `COLLIDER_TYPE`을 매개변수로 받아 대상의 타입을 확인하고, DirectX Math의 내장 충돌 검사 함수를 호출합니다.

---

## 성능 비교

### 계산 복잡도

| 충돌 타입         | 시간 복잡도 | 특징                                    |
|------------------|------------|----------------------------------------|
| AABB vs AABB     | O(1)       | 가장 빠름, 단순 좌표 비교                |
| Sphere vs Sphere | O(1)       | 매우 빠름, 거리 계산만 필요              |
| OBB vs OBB       | O(k)       | 상대적으로 느림, SAT 알고리즘 (k≈15)     |
| AABB/OBB vs Sphere| O(1)      | Sphere의 회전 독립성으로 인해 효율적    |

### 실제 프로젝트 사용

본 프로젝트에서는 주로 **Sphere 충돌 감지**를 사용합니다:

- 플레이어와 몬스터는 대부분 Sphere Collider 사용
- 환경 오브젝트와의 충돌도 주로 Sphere로 처리
- Sphere의 계산 효율성과 회전 독립성이 게임플레이에 적합

AABB와 OBB는 구현되어 있으나, 현재는 미사용 상태입니다. 향후 더 정밀한 충돌 검사가 필요한 경우 (예: 복잡한 지형, 회전하는 오브젝트) 활용 가능합니다.

---

## 디버그 렌더링

모든 Bounding 타입은 `#ifdef _DEBUG` 조건부 컴파일로 디버그 렌더링을 지원합니다:

```cpp
#ifdef _DEBUG
HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
    pBatch->Begin();
    DX::Draw(pBatch, *m_pDesc, vColor);  // DirectXTK의 Draw 함수 사용
    pBatch->End();
    return S_OK;
}
#endif
```

개발 중에는 Collision_Manager가 모든 Collider의 Bounding을 시각화하여 충돌 영역을 확인할 수 있습니다.

---

## 실제 사용 예시

### Collider 컴포넌트에서의 사용

```cpp
// Collider.cpp
_bool CCollider::Intersect(CCollider* pTargetCollider)
{
    // Bounding의 Intersect 메서드를 호출하여 실제 충돌 검사
    m_bIsCollision = m_pBounding->Intersect(
        pTargetCollider->m_eColliderType,  // 대상 Collider 타입
        pTargetCollider->m_pBounding        // 대상 Bounding 포인터
    );
    
    return m_bIsCollision;
}
```

### Collision_Manager에서의 통합

```cpp
// Collision_Manager::Update()
if (pColliderA->Intersect(pColliderB))
{
    pColliderA->Collided_With(pColliderB);
    pColliderB->Collided_With(pColliderA);
    // 충돌 처리 로직...
}
```

---

## 결론

본 프로젝트는 **세 가지 Bounding 타입을 모두 구현**하여 다양한 충돌 감지 요구사항에 대응할 수 있는 확장 가능한 시스템을 구축하였습니다:

1. **AABB**: 정적 오브젝트, 환경 구조물에 최적화
2. **OBB**: 회전하는 오브젝트의 정밀한 충돌 감지
3. **Sphere**: 동적 오브젝트, 실시간 충돌 검사에 최적화 (현재 주 사용)

각 타입은 DirectX Math 라이브러리의 최적화된 충돌 검사 함수를 활용하여 **효율적이고 정확한 충돌 감지**를 제공합니다.




## 실제 프로젝트 적용 - ColliderTag 기반 사용 사례

본 프로젝트에서는 **ColliderTag**를 활용하여 충돌 감지 시스템을 용도별로 세분화하여 관리합니다. 각 ColliderTag는 특정 목적을 위해 설계되었으며, 게임 로직에서 명확하게 구분하여 사용됩니다.

### 1. ColliderTag 분류 체계

| ColliderTag | 용도 | 설명 |
|-------------|------|------|
| **Player_Weapon** | 공격 감지 | 플레이어 무기 충돌, 몬스터에게 데미지 전달 |
| **Player_Body** | 상호작용/피킹 | 플레이어 충돌 체크, 마우스 피킹 대상 |
| **Player_Body_Small** | 물리 충돌 | 플레이어 밀어내기용, Add_Momentum 적용 |
| **Monster_Body_Hit** | 공격 감지 | 몬스터 피격 박스, 플레이어 공격 충돌 |
| **Monster_Body_NoHit** | 충돌만 체크 | 몬스터 피킹용, Ray 충돌 검사 대상, 데미지 무시 |
| **Monster_Body_Small** | 물리 충돌 | 몬스터 밀어내기용, Add_Momentum 적용 |
| **Player_Arrow** | 투사체 | 플레이어 화살 충돌 |
| **Monster_Arrow** | 투사체 | 몬스터 화살 충돌 |


### 2. 공격 감지 시스템

#### Player_Weapon vs Monster_Body_Hit

// State_Zombie.cpp - 몬스터가 플레이어 무기에 맞았을 때
_bool CState_Zombie::Modify_HP(CCollider* pOther)
{
    if (TEXT("Player_Weapon") == pOther->Get_ColliderTag()
        && pOther->Get_Other_Collision_Activated())
    {
        CItem* pItem = dynamic_cast<CItem*>(pOther->Get_OwnerObject());
        m_pMonsterInfo->Modify_CurrentHp(-pItem->Get_DealPoint());
        m_pZombie->Render_DamageFont(pItem->Get_DealPoint(), 4.f);
        return true;
    }
    // Player_Arrow, Monster_Weapon 등 다른 태그 처리...
}

**작동 원리**:
- `Player_Weapon` Collider는 무기 오브젝트에 부착
- `Monster_Body_Hit` Collider는 몬스터의 피격 가능 영역에 부착
- 충돌 시 `Collision_Enter`에서 ColliderTag 확인 후 데미지 처리

#### Player_Body vs Monster_Body_Hit

// State_Player.cpp - 플레이어가 몬스터에게 맞았을 때
void CState_Player::Modify_HP(CCollider* pOther)
{
    if (TEXT("Monster_Body_Hit") == pOther->Get_ColliderTag()
        && pOther->Get_Other_Collision_Activated())
    {
        CPartObject* pMonsterBody = dynamic_cast<CPartObject*>(pOther->Get_OwnerObject());
        CMonster* pMonster = dynamic_cast<CMonster*>(pMonsterBody->Get_ContainerObject());
        
        m_pPlayerInfo->Modify_CurrentHp(-pMonster->Get_MonsterInfo()->Get_DealPoint());
        // 데미지 폰트 렌더링...
    }
}

**설계 의도**
- **Monster_Body_Hit**: 공격 감지 가능, 플레이어가 공격 시 데미지 처리
- **Monster_Body_NoHit**: Ray 충돌 검사용 피킹 대상으로만 사용, 데미지 처리 안 함
  - 몬스터 피킹 시 충돌체가 필요하지만 데미지는 받지 않아야 하는 경우 (예: Vindicator, Skeleton 등)
  - 마우스로 몬스터를 선택하여 그 방향으로 향하는 기능에 활용


### 3. MouseRay 충돌 검사 (피킹)

Sphere Collider는 **Ray 충돌 검사**를 지원하여 마우스 피킹에 활용됩니다.
Ray 피킹은 상자, 아이템, 몬스터 등 다양한 오브젝트 선택에 사용됩니다.

// Player.cpp - 가장 가까운 Collider 찾기
CCollider* CPlayer::Get_Closest_Collider(const _float4& mousePos, const _float3& mouseRay)
{
	unordered_map<_wstring, vector<CCollider*>> colliders = *m_pGameInstance->Get_Colliders(m_pGameInstance->Get_CurrentLevelIndex());
	auto it = colliders.find(TEXT("Monster"));
	if (it == colliders.end())
		return nullptr;

	CCollider* pClosest = nullptr;
	_float minDist = FLT_MAX;

	for (auto& pCollider : it->second)
	{
		if (pCollider->Get_ColliderType() != COLLIDER_TYPE::TYPE_SPHERE || CCollider::COLLIDER_ROLE::BIG != pCollider->Get_Role())
			continue;

		_float fDist = 0.f;
		CBounding_Sphere::RayDesc rayDesc = {};
		rayDesc.MousePos = { mousePos.x, mousePos.y, mousePos.z };
		rayDesc.MouseRay = mouseRay;
		rayDesc.fDist = &fDist;

		if (pCollider->Get_Bounding()->Intersect(COLLIDER_TYPE::TYPE_RAY, nullptr, &rayDesc))
		{
			if (fDist < minDist)
			{
				minDist = fDist;
				pClosest = pCollider;
			}
		}
	}

	return pClosest;
}

**활용 사례**:
- 플레이어가 마우스로 상호작용할 오브젝트 선택
- 상자, 트리거 오브젝트 등과의 상호작용
- `Player_Body` 태그를 가진 오브젝트를 피킹하여 거리 확인



### 4. Small 태그와 Add_Momentum 시스템

#### 목적: 오브젝트 간 물리적 밀어내기

`_Small` 접미사가 붙은 ColliderTag는 **물리 충돌 처리**에 사용되며, `Collision_Manager`에서 자동으로 `Add_Momentum`을 적용합니다.

// Collision_Manager::Update()
if (pColliderA->Intersect(pColliderB))
{
    pColliderA->Collided_With(pColliderB);
    pColliderB->Collided_With(pColliderA);
    
    // SMALL vs SMALL 충돌 시 자동으로 밀어내기 적용
    if (pColliderA->Get_Role() == CCollider::SMALL && 
        pColliderB->Get_Role() == CCollider::SMALL && 
        pColliderA < pColliderB)
    {
        Resolve_Penetration_And_Slide(pColliderA, pColliderB, 9.f);
    }
}#### Resolve_Penetration_And_Slide 구현

// Collision_Manager::Resolve_Penetration_And_Slide()
void CCollision_Manager::Resolve_Penetration_And_Slide(CCollider* pColA, CCollider* pColB, _float fForce)
{
    // 두 Sphere의 중심점 계산
    _vector vPosA = ...;  // pColA의 중심점
    _vector vPosB = ...;  // pColB의 중심점
    
    // 충돌 방향 벡터 계산
    _vector vDir = XMVector3Normalize(vPosA - vPosB);
    
    // 밀어내기 임펄스 계산
    _vector vMomentumA = vDir * fForce;
    _vector vMomentumB = -vDir * fForce;
    
    // 각 오브젝트에 Momentum 적용
    pColA->Get_OwnerObject()->Apply_Penetration_Momentum(vMomentumA);
    pColB->Get_OwnerObject()->Apply_Penetration_Momentum(vMomentumB);
}

#### Transform::Add_Momentum

// Transform.cpp
void CTransform::Add_Momentum(_vector vImpulse)
{
    _vector vOld = XMLoadFloat3(&m_Velocity);
    vOld += vImpulse;  // 속도에 임펄스 추가
    XMStoreFloat3(&m_Velocity, vOld);
}

void CTransform::Update_Momentum(_float fTimeDelta)
{
    // Velocity를 위치에 적용
    _vector vCurPos = Get_State(STATE_POSITION);
    _vector vNextPos = vCurPos + vVel * fTimeDelta;
    
    // Navigation을 고려한 이동 처리
    if (m_pNavigationCom && m_pNavigationCom->Can_Move(vNextPos))
    {
        Set_State(STATE_POSITION, vNextPos);
    }
    // 슬라이딩 처리...

}


#### 실제 사용 예시

// Player_Arrow.cpp - 화살이 몬스터 Small Collider에 맞았을 때
void CPlayer_Arrow::Collided_With(CCollider* pOther, CCollider::COLLISION_STATE eCollisionState)
{
    if (pOther->Is_ColliderActive() &&
        CCollider::COLLISION_STATE::ENTER == eCollisionState &&
        (TEXT("Monster_Body_Small") == pOther->Get_ColliderTag() || 
         TEXT("Boss_Body_Small") == pOther->Get_ColliderTag()))
    {
        m_bCollided = true;
        // 화살 충돌 처리...
    }
}


### 5. ColliderTag 등록 시스템

// Body_Player.cpp - 플레이어 Collider 등록
HRESULT CBody_Player::Ready_Components()
{
    // 큰 충돌 박스 (공격 감지, 상호작용용)
    m_pGameInstance->Add_ColliderCom(
        m_pGameInstance->Get_ChangedLevelIndex(),
        pColliderBigSphereCom,
        TEXT("Player_Body"),      // ColliderTag
        TEXT("Player"),           // ObjectType (그룹)
        true                      // Persistent
    );
    
    // 작은 충돌 박스 (물리 충돌, 밀어내기용)
    m_pGameInstance->Add_ColliderCom(
        m_pGameInstance->Get_ChangedLevelIndex(),
        m_pSmallColliderCom,
        TEXT("Player_Body_Small"), // ColliderTag
        TEXT("Player"),            // ObjectType
        true                       // Persistent
    );
}

**설계 의도**: 
- **BIG Collider**: 공격 감지, 상호작용 등 게임 로직용 (더 큰 범위)
- **SMALL Collider**: 물리 충돌, 밀어내기용 (정밀한 충돌)

---

## 기술적 장점

### 1. 태그 기반 분리
- ColliderTag로 용도를 명확히 구분하여 코드 가독성 향상
- 동일 오브젝트에 여러 용도의 Collider를 부착 가능

### 2. 자동화된 물리 처리
- SMALL vs SMALL 충돌 시 자동으로 `Add_Momentum` 적용
- Collision_Manager에서 일괄 처리하여 코드 중복 제거

### 3. 유연한 충돌 관리
- `Monster_Body_Hit` vs `Monster_Body_NoHit`로 데미지 처리 분리
- 각 State에서 필요한 ColliderTag만 처리하여 성능 최적화

### 4. Ray 충돌 검사
- Sphere의 Ray 충돌 검사로 마우스 피킹 구현
- 거리 정보를 반환받아 가장 가까운 오브젝트 선택 가능