#include "Vindicator_Attack.h"
#include "Vindicator.h"

#define VINDICATOR_ATTACKSTART  15.f
#define VINDICATOR_ATTACKFINISH  24.f

CVindicator_Attack::CVindicator_Attack(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_Vindicator(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CVindicator_Attack::Init_State()
{
	__super::Init_State();

    m_pVindicator = dynamic_cast<CVindicator*>(m_pActor);
	if (nullptr == m_pVindicator)
        return E_FAIL;

	return S_OK;
}

void CVindicator_Attack::State_Enter()
{
    m_fAnimTimer = 0.f;
    m_bHitMode_Activated = false;

    if (nullptr == m_pAxeCollider)
        m_pAxeCollider = dynamic_cast<CCollider*>(m_pVindicator->Find_Part_Component(TEXT("Part_Weapon_Melee"),
            TEXT("Com_Collider_Sphere")));

	m_pActorModelCom->Set_Animation(static_cast<_uint>(VINDICATOR_STATE::ATTACK), false, 0.9f);
}

void CVindicator_Attack::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CVindicator_Attack::State_Update(_float fTimeDelta)
{
    __super::State_Update(fTimeDelta);

    _float  fAnimCurTrackPos = m_pActorModelCom->Get_AnimCurrentTrackPosition();

    if (!m_bHitMode_Activated && VINDICATOR_ATTACKSTART <= fAnimCurTrackPos)
    {
        m_pVindicator->Set_Attacking(true);
        m_bHitMode_Activated = true;
        m_pAxeCollider->Set_ColliderActive(true);
    }

    if (m_bHitMode_Activated && fAnimCurTrackPos >= VINDICATOR_ATTACKFINISH)
    {
        m_bHitMode_Activated = false; // 다시 사용할 수 있게
        m_pVindicator->Set_Attacking(false);
        m_pAxeCollider->Set_ColliderActive(false);
    }

    // 애니메이션 끝나면 상태 전환
    if (m_bAnimationFinished)
    {
        /*if (Change_State_To_Idle())
            return;*/

        if (Change_State_To_Walk())
            return;

        /*m_fAnimTimer = 0.f;
        m_bHitMode_Activated = false;*/
    }

    if (false == m_pTransformCom->Get_Is_Jumping())
    {
        _float4 playerPos = m_pVindicator->Get_Player_Position(TEXT("GameObject_Player"), m_pGameInstance->Get_CurrentLevelIndex());
        m_pTransformCom->LookAt(XMLoadFloat4(&playerPos));
    }
}


void CVindicator_Attack::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CVindicator_Attack::State_Exit()
{
    m_pVindicator->Set_Retreating(true);
}

void CVindicator_Attack::Collision_Enter(CCollider* pOther)
{
    __super::Collision_Enter(pOther);

    Modify_HP(pOther);
}

void CVindicator_Attack::Collision_Stay(CCollider* pOther)
{
    __super::Collision_Stay(pOther);
}

void CVindicator_Attack::Collision_Exit(CCollider* pOther)
{
    __super::Collision_Exit(pOther);
}

CState_Monster* CVindicator_Attack::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
{
    CVindicator_Attack* pGameInstance = new CVindicator_Attack(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CVindicator_Attack");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CVindicator_Attack::Free()
{
	__super::Free();
}
