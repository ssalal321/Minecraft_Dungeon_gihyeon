#include "BabyZombie_Dead.h"
#include "BabyZombie.h"

#include "AnimationCurve.h"

CBabyZombie_Dead::CBabyZombie_Dead(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_BabyZombie(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CBabyZombie_Dead::Init_State()
{
	__super::Init_State();

	m_pBabyZombie = dynamic_cast<CBabyZombie*>(m_pActor);
	if (nullptr == m_pBabyZombie)
		return E_FAIL;

	m_fCurrentTime  = 0.f;
	m_fDurationTime = 3.f;

	return S_OK;
}

void CBabyZombie_Dead::State_Enter()
{
	//m_pActorModelCom->Set_Animation(static_cast<_uint>(BABYZOMBIE_STATE::IDLE), false, 1.0f);
}

void CBabyZombie_Dead::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CBabyZombie_Dead::State_Update(_float fTimeDelta)
{
	// Play_Animation 안할랫
	//__super::State_Update(fTimeDelta);

	//옆으로 먼저 쓰러진다음!
	if (!m_bDowned)
	{
		m_pTransformCom->Rotation({ 0.f, 0.f, 1.f }, 10.f);

		// 90도 기울었으면 쓰러졌다고 판단
		if (XMVectorGetX(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) >= 90.f)
		{
			m_bDowned = true;
		}

		return;
	}


	if (false == m_pBabyZombie->Get_Can_be_Eaten())
	{
		m_pActor->Set_GameObject_Active(false);
		m_pBigColliderCom->Set_ColliderActive(false);
		m_pSmallColliderCom->Set_ColliderActive(false);
	}

	else if (true == m_pBabyZombie->Get_Can_be_Eaten())
	{
		// 빨려들어갈 목표 위치 (예: CauldronBoss 위치)
		CTransform*		pBossTransformCom = dynamic_cast<CTransform*>(m_pBabyZombie->Get_Eating_BossMonster()->Find_Component(TEXT("Com_Transform")));
		_vector			vTargetPos = pBossTransformCom->Get_State(CTransform::STATE_POSITION);  // CauldronBoss 위치를 목표로 설정
		_float			fSpeed = 2.0f; // 빨려들어가는 속도

		// EaseOutBack 커브를 사용하여 점차적으로 빨려들어가도록 설정
		_float	fProgressRatio = m_fCurrentTime / m_fDurationTime;  // 0 ~ 1
		_float	fEaseOutBackProgress = CAnimationCurve::EaseOutBack(fProgressRatio);

		// 목표 위치로 이동 (커브 적용)
		 // 이동 거리 계산 (목표 위치와 현재 위치의 차이)
		_vector		currentPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector		direction = vTargetPos - currentPosition;

		// 이동 거리에 속도와 커브를 반영하여 위치 계산
		currentPosition = currentPosition + direction * fEaseOutBackProgress * fSpeed;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, currentPosition);

		// 이동을 계속하면서 점차적으로 목표에 가까워짐
		m_fCurrentTime += fTimeDelta;
		if (m_fCurrentTime >= m_fDurationTime)
		{
			// 이동이 완료되었으면, 추가적인 처리
			m_bCurveFinished = true;
		}
	}
}

void CBabyZombie_Dead::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CBabyZombie_Dead::State_Exit()
{
}

void CBabyZombie_Dead::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);

	//빨려들어가다가 CauldronBoss랑 부딪히면 먹히고 사라지기
	if (m_bCurveFinished && pOther->Get_ColliderTag() == TEXT("Boss_Body_Small"))
	{
		m_pActor->Set_GameObject_Active(false);
		m_pBigColliderCom->Set_ColliderActive(false);
		m_pSmallColliderCom->Set_ColliderActive(false);
	}
}

void CBabyZombie_Dead::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CBabyZombie_Dead::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Monster* CBabyZombie_Dead::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
{
	CBabyZombie_Dead* pGameInstance = new CBabyZombie_Dead(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CBabyZombie_Dead");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CBabyZombie_Dead::Free()
{
	__super::Free();
}
