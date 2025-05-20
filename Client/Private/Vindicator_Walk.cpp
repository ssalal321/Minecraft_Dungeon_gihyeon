#include "Vindicator_Walk.h"
#include "Vindicator.h"

#include "Player.h"

#define RETREAT_DISTANCE 5.f

CVindicator_Walk::CVindicator_Walk(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_Vindicator(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CVindicator_Walk::Init_State()
{
	__super::Init_State();

	m_pVindicator = dynamic_cast<CVindicator*>(m_pActor);
	if (nullptr == m_pVindicator)
		return E_FAIL;

	return S_OK;
}

void CVindicator_Walk::State_Enter()
{
	__super::State_Enter();

	m_pActorModelCom->Set_Animation(static_cast<_uint>(VINDICATOR_STATE::WALK), true, 1.3f);

	if (m_pVindicator->Get_Retreating())
	{
		// 후퇴 방향 : 플레이어 반대 방향
		_vector vecToPlayer = m_pVindicator->Vec_To_Player(TEXT("GameObject_Player"),
															m_pGameInstance->Get_CurrentLevelIndex());
		m_vRetreatDir = XMVector3Normalize(-vecToPlayer);

		XMStoreFloat4(&m_vRetreatStartPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
}

void CVindicator_Walk::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CVindicator_Walk::State_Update(_float fTimeDelta)
{
    __super::State_Update(fTimeDelta);

    // ** 1) 플레이어 벗어나면 Idle **
    if (Change_State_To_Idle())
        return;

    // ** 2) 후퇴 모드라면, 먼저 후퇴 **
    if (m_pVindicator->Get_Retreating())
    {
        // 후퇴
        _vector curPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        _vector targetPos = XMVectorAdd(curPos, m_vRetreatDir);
        m_pTransformCom->LookAt(targetPos);
        m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);

        // 얼마나 후퇴했나 측정
        _vector start = XMLoadFloat4(&m_vRetreatStartPos);
        _vector moved = XMVectorSubtract(curPos, start);
        _float dist = XMVectorGetX(XMVector3Length(moved));
        if (dist >= RETREAT_DISTANCE || false == m_pNavigationCom->Can_Move(start + moved))
        {
            // 후퇴 완료
            m_pVindicator->Set_Retreating(false);
        }
        return;
    }

    // ** 3) 공격 가능 거리면 Attack 으로 진입 **
    _float lengthToPlayer = m_pVindicator->Length_To_Player();

	if (lengthToPlayer <= m_pVindicator->Get_MonsterInfo()->fAttackableRange)
    {
        m_pVindicator->Change_State(Make_VindicatorState(VINDICATOR_STATE::ATTACK));
        return;
    }

    // ** 4) 그 외에는 플레이어 쫓아가기 **
    _float4 playerPos = m_pVindicator->Get_Player_Position(TEXT("GameObject_Player"),
															m_pGameInstance->Get_CurrentLevelIndex());
    m_pTransformCom->LookAt(XMLoadFloat4(&playerPos));
    m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
}

void CVindicator_Walk::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CVindicator_Walk::State_Exit()
{
}

void CVindicator_Walk::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);

    Modify_HP(pOther);
}

void CVindicator_Walk::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CVindicator_Walk::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

CState_Monster* CVindicator_Walk::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
{
	CVindicator_Walk* pGameInstance = new CVindicator_Walk(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CVindicator_Walk");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CVindicator_Walk::Free()
{
	__super::Free();
}
