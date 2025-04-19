#include "Zombie_Attack.h"

#include "Zombie.h"

CZombie_Attack::CZombie_Attack(CGameObject* pActor, CModel* pZombieModelCom, CCollider* pColliderCom,
								CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
								CTransform* pTransformCom, CNavigation* pNavigationCom)
	: CState_Zombie(pActor, pZombieModelCom, pColliderCom, pGameObjectDesc, pTransformCom, pNavigationCom)
{
}

HRESULT CZombie_Attack::Init_State()
{
	__super::Init_State();

	m_pZombie = dynamic_cast<CZombie*>(m_pActor);

	return S_OK;
}

void CZombie_Attack::State_Enter()
{
    m_fAnimTimer = 0.f;
    m_bHitbox_Activated = false;

	m_pActorModelCom->Set_Animation(static_cast<_uint>(ZOMBIE_STATE::ATTACK), true);
}

void CZombie_Attack::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}


void CZombie_Attack::State_Update(_float fTimeDelta)
{
    __super::State_Update(fTimeDelta);

    m_fAnimTimer += fTimeDelta;

    CBounding_OBB* pBoundingOBB = dynamic_cast<CBounding_OBB*>(m_pColliderCom->Get_Bounding());

    // 0.5초 지났을 때 공격 콜라이더 활성화 (1회만)
    if (!m_bHitbox_Activated && m_fAnimTimer > 0.5f)
    {
        m_pZombie->Set_Attacking(true);
        pBoundingOBB->Edit_Bounding_Extent({ 0.f, 0.f, 0.4f });
        pBoundingOBB->Edit_Bounding_Center({ 0.f, 0.f, 0.4f });
        m_bHitbox_Activated = true;
    }

    // 1.0초 쯤 다시 초기화
    if (m_bHitbox_Activated && m_fAnimTimer > 1.0f)
    {
        pBoundingOBB->Edit_Bounding_Extent({ 0.f, 0.f, -0.4f });
        pBoundingOBB->Edit_Bounding_Center({ 0.f, 0.f, -0.4f });
        m_bHitbox_Activated = false; // 다시 사용할 수 있게
        m_pZombie->Set_Attacking(false);
    }

    // 애니메이션 끝나면 상태 전환
    if (m_bAnimationFinished)
    {
        if (Change_State_To_Idle())
            return;

        if (Change_State_To_Walk())
            return;

        m_fAnimTimer = 0.f;
        m_bHitbox_Activated = false;
    }

    _float4 playerPos = m_pZombie->Get_Player_Position(TEXT("Prototype_GameObject_PlayerHex"), LEVEL_GAMEPLAY);
    m_pTransformCom->LookAt(XMLoadFloat4(&playerPos));
}


void CZombie_Attack::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CZombie_Attack::State_Exit()
{
}

void CZombie_Attack::Collision_Enter(CCollider* pOther)
{
	
}

void CZombie_Attack::Collision_Stay(CCollider* pOther)
{

}

void CZombie_Attack::Collision_Exit(CCollider* pOther)
{

}

CState_Monster* CZombie_Attack::Create(CGameObject* pActor, CModel* pZombieModelCom, CCollider* pColliderCom,
										CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
										CTransform* pTransformCom, CNavigation* pNavigationCom)
{
	CZombie_Attack* pGameInstance = new CZombie_Attack(pActor, pZombieModelCom, pColliderCom, pGameObjectDesc, pTransformCom, pNavigationCom);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CZombie_Attack");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CZombie_Attack::Free()
{
	__super::Free();
}
