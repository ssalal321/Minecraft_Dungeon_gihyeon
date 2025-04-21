#pragma once
#include "Client_Defines.h"
#include "State_Monster.h"

BEGIN(Client)
class CSkeleton;

class CState_Skeleton abstract: public CState_Monster
{
protected:
    CState_Skeleton(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc);
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

protected:
    _bool     Change_State_To_Attack();
    _bool     Change_State_To_Walk();
    _bool     Change_State_To_Idle();
    _bool     Change_State_To_GetHit(CCollider* pOther);

public:
    void   Free()    override;
};

END