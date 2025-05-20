#include "Zombie_Dead.h"

#include <random>

#include "Zombie.h"

#include "AnimationCurve.h"

CZombie_Dead::CZombie_Dead(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_Zombie(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CZombie_Dead::Init_State()
{
	__super::Init_State();

	m_pZombie = dynamic_cast<CZombie*>(m_pActor);
	if (nullptr == m_pZombie)
		return E_FAIL;

	m_fCurrentTime  = 0.f;
	m_fDurationTime = 3.f;

	return S_OK;
}

void CZombie_Dead::State_Enter()
{
	m_pActorModelCom->Set_Animation(static_cast<_uint>(BABYZOMBIE_STATE::IDLE), false, 1.0f);

	// 고품질 랜덤 방향 설정
	std::random_device rd;
	std::mt19937 gen(rd());
	uniform_int_distribution<_int> dist(0, 3);

	_int dir = dist(gen);

	switch (dir)
	{
	case 0:
		m_vFallAxis = XMVectorSet(-1.f, 0.f, 0.f, 0.f); // -X
		break;
	case 1:
		m_vFallAxis = XMVectorSet(1.f, 0.f, 0.f, 0.f);  // +X
		break;
	case 2:
		m_vFallAxis = XMVectorSet(0.f, 0.f, -1.f, 0.f); // -Z
		break;
	case 3:
		m_vFallAxis = XMVectorSet(0.f, 0.f, 1.f, 0.f);  // +Z
		break;
	}

	m_fAccumulatedRotation = 0.f;
	m_bDowned = false;
}

void CZombie_Dead::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CZombie_Dead::State_Update(_float fTimeDelta)
{
	// Play_Animation 안할랫
	//__super::State_Update(fTimeDelta);

	//옆으로 먼저 쓰러진다음!
	if (!m_bDowned)
	{
		// 누적 회전량 (라디안) 계산
		m_fAccumulatedRotation += XMConvertToRadians(m_pTransformCom->Get_Rotation_perSec() * fTimeDelta);

		// Z축 회전 수행 (라디안 단위)
		m_pTransformCom->Rotation(m_vFallAxis, XMConvertToRadians(m_pTransformCom->Get_Rotation_perSec() * fTimeDelta));

		// 90도(= π/2 라디안) 이상 회전했으면 멈춤
		if (m_fAccumulatedRotation >= XM_PIDIV2)
		{
			m_bDowned = true;
		}

		return;
	}

	if (!m_bDelayFinished)
	{
		m_fDelayTime += fTimeDelta;

		if (m_fDelayTime >= 2.f)
			m_bDelayFinished = true;

		return;
	}

	if (false == m_pZombie->Get_Can_be_Eaten())
	{
		m_pActor->Set_GameObject_Active(false);
		m_pBigColliderCom->Set_ColliderActive(false);
		m_pSmallColliderCom->Set_ColliderActive(false);
	}

	else if (true == m_pZombie->Get_Can_be_Eaten())
	{
		// 빨려들어갈 목표 위치 (예: CauldronBoss 위치)
		CTransform*		pBossTransformCom = dynamic_cast<CTransform*>(m_pZombie->Get_Eating_BossMonster()->Find_Component(TEXT("Com_Transform")));
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

void CZombie_Dead::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CZombie_Dead::State_Exit()
{
}

void CZombie_Dead::Collision_Enter(CCollider* pOther)
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

void CZombie_Dead::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CZombie_Dead::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Monster* CZombie_Dead::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
{
	CZombie_Dead* pGameInstance = new CZombie_Dead(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CZombie_Dead");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CZombie_Dead::Free()
{
	__super::Free();
}
