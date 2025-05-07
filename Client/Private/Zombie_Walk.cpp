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
    std::random_device random;  // 진정한 난수 생성기
    std::mt19937 gen(random()); // Mersenne Twister 엔진에 rd()로 시드 설정
    std::uniform_int_distribution<int> dist(0, 7); // 0부터 4까지의 균일 분포

    _int    randomNum = dist(gen);   // 0부터 7 사이의 난수
    _float  angle = randomNum * 45.f; // 0º, 45º, 90º, 135º, 180º, 225º, 270º, 315º

    // 3. 이동할 거리 선택
    _float   distances[] = { 3.f, 3.5f, 4.f };
    m_fDistance = distances[rand() % 3];  // 0 ~ 2 -> 3, 4, 5

    // 4. 방향 벡터 계산 및 정규화
    m_vRandomWalkDir.m128_f32[0] = cosf(XMConvertToRadians(angle));  // x
    m_vRandomWalkDir.m128_f32[2] = sinf(XMConvertToRadians(angle));  // z
    m_vRandomWalkDir.m128_f32[1] = 0.f;  // y
    XMVector3Normalize(m_vRandomWalkDir);

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
