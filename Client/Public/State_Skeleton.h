#pragma once
#include "Client_Defines.h"
#include "State_Monster.h"

BEGIN(Client)
class CSkeleton;
class CArrowPool_Monster;

class CState_Skeleton abstract: public CState_Monster
{
public:
    typedef struct tagStateSkeleton : STATEMONSTER_DESC
    {
        CArrowPool_Monster*     pArrowPool_Monster = { nullptr };

    }STATE_SKELETON_DESC;

protected:
    CState_Skeleton(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATE_SKELETON_DESC* pDesc);
	~CState_Skeleton() override = default;

public:
    HRESULT   Init_State()                              override;

    void      State_Enter()                             override;
    void      State_Priority_Update(_float fTimeDelta)  override;
    void      State_Update(_float fTimeDelta)           override;
    void      State_Late_Update(_float fTimeDelta)      override;
    void      State_Exit()                              override;

    void      Collision_Enter(CCollider* pOther)      override;
    void      Collision_Stay(CCollider* pOther)       override;
    void      Collision_Exit(CCollider* pOther)       override;

protected:
    CSkeleton*    m_pSkeleton = { nullptr };

    CArrowPool_Monster*     m_pArrowPool_Monster = { nullptr };;

protected:
    _bool     Change_State_To_Attack();
    _bool     Change_State_To_Walk();
    _bool     Change_State_To_Idle();
    _bool     Change_State_To_HeadSpin();
	_bool     Change_State_To_GetHit();
    void      Modify_HP(CCollider* pOther);

public:
    void   Free()    override;
};

END