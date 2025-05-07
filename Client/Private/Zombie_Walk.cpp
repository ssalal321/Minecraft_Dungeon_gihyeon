#include "Zombie_Walk.h"

#include <random>

#include "Player.h"
#include "Zombie.h"

CZombie_Walk::CZombie_Walk(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_Zombie(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CZombie_Walk::Init_State()
{
	__super::Init_State();

	m_pZombie = dynamic_cast<CZombie*>(m_pActor);
	if (nullptr == m_pZombie)
		return E_FAIL;

	return S_OK;
}

void CZombie_Walk::State_Enter()
{
    m_bLostPlayer = false;

	m_pActorModelCom->Set_Animation(static_cast<_uint>(ZOMBIE_STATE::WALK), true);
}

void CZombie_Walk::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CZombie_Walk::State_Update(_float fTimeDelta)
{
    __super::State_Update(fTimeDelta);

    if (Change_State_To_Attack())
        return;

    if (!m_pZombie->Player_In_DetectRange())
    {
        if (!m_bLostPlayer)
        {
            Direction_Setting();
            m_bLostPlayer = true;
        }

        m_pTransformCom->LookAt(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_vRandomWalkDir);
        m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);

        _vector vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        _vector vMoved = vCurPos - m_vWalkStartPos;

        _float fMovedDist = {};
        XMStoreFloat(&fMovedDist, XMVector3Length(vMoved));

        if (fMovedDist >= m_fDistance)
        {
            m_pZombie->Change_State(Make_ZombieState(ZOMBIE_STATE::IDLE));
            return;
        }

        return;
    }

    // 플레이어가 감지 범위에 있을 경우 정상 이동
    m_bLostPlayer = false;

    _float4 playerPos = m_pZombie->Get_Player_Position(TEXT("GameObject_Player"), m_pGameInstance->Get_CurrentLevelIndex());
    m_pTransformCom->LookAt(XMLoadFloat4(&playerPos));
    m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
}

void CZombie_Walk::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CZombie_Walk::State_Exit()
{
}

void CZombie_Walk::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);

	Change_State_To_GetHit(pOther);
}

void CZombie_Walk::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CZombie_Walk::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

void CZombie_Walk::Direction_Setting()
{
    std::random_device random;
    std::mt19937 gen(random());
    std::uniform_int_distribution<_int> angleDist(0, 7);
    std::uniform_int_distribution<_int> distIndex(0, 2);

    _int randomNum = angleDist(gen);
    _float angle = randomNum * 45.f;

    _float distances[] = { 2.f, 2.5f, 3.f };
    m_fDistance = distances[distIndex(gen)];

    m_vRandomWalkDir = XMVectorSet(cosf(XMConvertToRadians(angle)), 0.f, sinf(XMConvertToRadians(angle)), 0.f );
    m_vRandomWalkDir = XMVector3Normalize(m_vRandomWalkDir);

    m_vWalkStartPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
}

CState_Monster* CZombie_Walk::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
{
	CZombie_Walk* pGameInstance = new CZombie_Walk(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CZombie_Walk");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CZombie_Walk::Free()
{
	__super::Free();
}
