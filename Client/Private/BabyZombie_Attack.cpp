#include "BabyZombie_Attack.h"
#include "BabyZombie.h"

#define BABYZOMBIE_ATTACKSTART  11.f
#define BABYZOMBIE_ATTACKFINISH  14.f

CBabyZombie_Attack::CBabyZombie_Attack(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_BabyZombie(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CBabyZombie_Attack::Init_State()
{
	__super::Init_State();

    m_pBabyZombie = dynamic_cast<CBabyZombie*>(m_pActor);
	if (nullptr == m_pBabyZombie)
        return E_FAIL;

	return S_OK;
}

void CBabyZombie_Attack::State_Enter()
{
    m_fAnimTimer = 0.f;
    m_bHitMode_Activated = false;

	m_pActorModelCom->Set_Animation(static_cast<_uint>(BABYZOMBIE_STATE::ATTACK), false);
}

void CBabyZombie_Attack::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CBabyZombie_Attack::State_Update(_float fTimeDelta)
{
    __super::State_Update(fTimeDelta);

    _float  fAnimCurTrackPos = m_pActorModelCom->Get_AnimCurrentTrackPosition();

    if (!m_bHitMode_Activated && BABYZOMBIE_ATTACKSTART <= fAnimCurTrackPos)
    {
        m_pBabyZombie->Set_Attacking(true);
        m_bHitMode_Activated = true;
    }

    if (m_bHitMode_Activated && fAnimCurTrackPos >= BABYZOMBIE_ATTACKFINISH)
    {
        m_bHitMode_Activated = false; // 다시 사용할 수 있게
        m_pBabyZombie->Set_Attacking(false);
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

    _float4 playerPos = m_pBabyZombie->Get_Player_Position(TEXT("GameObject_Player"), m_pGameInstance->Get_CurrentLevelIndex());
    m_pTransformCom->LookAt(XMLoadFloat4(&playerPos));
}


void CBabyZombie_Attack::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CBabyZombie_Attack::State_Exit()
{
   /* CBounding_Sphere* pBoundingSphere = dynamic_cast<CBounding_Sphere*>(m_pColliderCom->Get_Bounding());
    pBoundingSphere->ReSet_Bounding_Radius(1.5f);
    pBoundingSphere->ReSet_Bounding_Center({ 0.f, 1.5f, 0.f });*/

    m_pBabyZombie->Set_Retreating(true);
}

void CBabyZombie_Attack::Collision_Enter(CCollider* pOther)
{
    __super::Collision_Enter(pOther);

    Change_State_To_GetHit(pOther);
}

void CBabyZombie_Attack::Collision_Stay(CCollider* pOther)
{
    __super::Collision_Stay(pOther);
}

void CBabyZombie_Attack::Collision_Exit(CCollider* pOther)
{
    __super::Collision_Exit(pOther);
}

CState_Monster* CBabyZombie_Attack::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
{
    CBabyZombie_Attack* pGameInstance = new CBabyZombie_Attack(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CBabyZombie_Attack");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CBabyZombie_Attack::Free()
{
	__super::Free();
}
