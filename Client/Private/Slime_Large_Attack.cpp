#include "Slime_Large_Attack.h"
#include "Slime_Large.h"

#define ATTACKSTART 5.f
#define ATTACKFINISH 8.f

CSlime_Large_Attack::CSlime_Large_Attack(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_Slime_Large(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CSlime_Large_Attack::Init_State()
{
	__super::Init_State();

	m_pSlime_Large = dynamic_cast<CSlime_Large*>(m_pActor);
	if (nullptr == m_pSlime_Large)
        return E_FAIL;

	return S_OK;
}

void CSlime_Large_Attack::State_Enter()
{
    m_fCoolTime = 0.f;
    m_bHitMode_Activated = false;

	m_pActorModelCom->Set_Animation(static_cast<_uint>(SLIME_LARGE_STATE::ATTACK), false, 1.f);
}

void CSlime_Large_Attack::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CSlime_Large_Attack::State_Update(_float fTimeDelta)
{
    __super::State_Update(fTimeDelta);

    if (m_bAttackFinished)
        m_fCoolTime += fTimeDelta;

    _float fAnimCurTrackPos = m_pActorModelCom->Get_AnimCurrentTrackPosition();

    if (!m_bHitMode_Activated && ATTACKSTART <= fAnimCurTrackPos)
    {
        m_pSlime_Large->Set_Attacking(true);
        m_bHitMode_Activated = true;
    }

    if (m_bHitMode_Activated && fAnimCurTrackPos >= ATTACKFINISH)
    {
        m_bHitMode_Activated = false; // 다시 사용할 수 있게
        m_pSlime_Large->Set_Attacking(false);
    }

    // 애니메이션 끝나면 상태 전환
    if (m_bAnimationFinished && !m_bAttackFinished)
    {
        m_bAttackFinished = true;
        m_bHitMode_Activated = false;
        m_fCoolTime = 0.f;
    }

    if (m_fCoolTime > 1.5f)
    {
        if (Change_State_To_Walk())
            return;
    }

    _float4  playerPos = m_pSlime_Large->Get_Player_Position(TEXT("GameObject_Player"), m_pGameInstance->Get_CurrentLevelIndex());
    m_pTransformCom->LookAt(XMLoadFloat4(&playerPos));
}


void CSlime_Large_Attack::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CSlime_Large_Attack::State_Exit()
{
}

void CSlime_Large_Attack::Collision_Enter(CCollider* pOther)
{
    __super::Collision_Enter(pOther);

    Change_State_To_Stun(pOther);
}

void CSlime_Large_Attack::Collision_Stay(CCollider* pOther)
{
    __super::Collision_Stay(pOther);
}

void CSlime_Large_Attack::Collision_Exit(CCollider* pOther)
{
    __super::Collision_Exit(pOther);
}

CState_Monster* CSlime_Large_Attack::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
{
	CSlime_Large_Attack* pGameInstance = new CSlime_Large_Attack(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CSlime_Large_Attack");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CSlime_Large_Attack::Free()
{
	__super::Free();
}
