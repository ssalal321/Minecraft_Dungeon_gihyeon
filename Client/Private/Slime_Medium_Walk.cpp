#include "Slime_Large_Walk.h"
#include "Slime_Large.h"

#include <random>
#include "Player.h"

CSlime_Large_Walk::CSlime_Large_Walk(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
	: CState_Slime_Large(pActor, pGameObjectDesc, pDesc)
{
}

HRESULT CSlime_Large_Walk::Init_State()
{
	__super::Init_State();

	m_pSlime_Large = dynamic_cast<CSlime_Large*>(m_pActor);
	if (nullptr == m_pSlime_Large)
		return E_FAIL;

	return S_OK;
}

void CSlime_Large_Walk::State_Enter()
{
    m_bLostPlayer = false;

    m_pTransformCom->Jump_Start(7.f);
	m_pActorModelCom->Set_Animation(static_cast<_uint>(SLIME_LARGE_STATE::WALK), true, 0.7f);
}

void CSlime_Large_Walk::State_Priority_Update(_float fTimeDelta)
{
	__super::State_Priority_Update(fTimeDelta);
}

void CSlime_Large_Walk::State_Update(_float fTimeDelta)
{
    __super::State_Update(fTimeDelta);

    if (Change_State_To_Attack())
        return;

    // 항상 점프
    if (!m_pTransformCom->Get_Is_Jumping())
    {
        m_pTransformCom->Jump_Start(7.f); // 또는 원하는 점프 초기 속도
    }

    m_pTransformCom->Jump(fTimeDelta, m_pNavigationCom);
	m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);

    
    // 분기 1: 플레이어를 탐지하지 못한 경우 랜덤 방향으로 걷기
    if (!m_pSlime_Large->Player_In_DetectRange())
    {
        if (!m_bLostPlayer)
        {
            Direction_Setting();
            m_bLostPlayer = true;
        }

        _vector vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        _vector vMoved = vCurPos - m_vWalkStartPos;

        _float fMovedDist = {};
        XMStoreFloat(&fMovedDist, XMVector3Length(vMoved));

        if (fMovedDist >= m_fDistance)
        {
            m_pSlime_Large->Change_State(Make_Slime_LargeState(SLIME_LARGE_STATE::IDLE));
        }
    }
    else
    {
        // 분기 2: 플레이어가 감지 범위에 있는 경우

        m_bLostPlayer = false;

        _float4 playerPos = m_pSlime_Large->Get_Player_Position(TEXT("GameObject_Player"), m_pGameInstance->Get_CurrentLevelIndex());
        m_pTransformCom->LookAt(XMLoadFloat4(&playerPos));

    }
    
}


void CSlime_Large_Walk::State_Late_Update(_float fTimeDelta)
{
	__super::State_Late_Update(fTimeDelta);
}

void CSlime_Large_Walk::State_Exit()
{
}

void CSlime_Large_Walk::Collision_Enter(CCollider* pOther)
{
	__super::Collision_Enter(pOther);

	Change_State_To_Stun(pOther);
}

void CSlime_Large_Walk::Collision_Stay(CCollider* pOther)
{
	__super::Collision_Stay(pOther);
}

void CSlime_Large_Walk::Collision_Exit(CCollider* pOther)
{
	__super::Collision_Exit(pOther);
}

void CSlime_Large_Walk::Direction_Setting()
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
    m_pTransformCom->LookAt(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_vRandomWalkDir);

    m_vWalkStartPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
}

CState_Monster* CSlime_Large_Walk::Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc)
{
	CSlime_Large_Walk* pGameInstance = new CSlime_Large_Walk(pActor, pGameObjectDesc, pDesc);

	if (FAILED(pGameInstance->Init_State()))
	{
		MSG_BOX("Failed to Create : CSlime_Large_Walk");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CSlime_Large_Walk::Free()
{
	__super::Free();
}
