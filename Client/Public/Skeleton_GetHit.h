#pragma once
#include "Client_Defines.h"
#include "State_Skeleton.h"

BEGIN(Client)

class CSkeleton_GetHit final: public CState_Skeleton
{
private:
    CSkeleton_GetHit(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATE_SKELETON_DESC* pDesc);
	~CSkeleton_GetHit() override = default;

public:
    HRESULT     Init_State()                                override;

    void        State_Enter()                               override;
    void        State_Priority_Update(_float fTimeDelta)    override;
    void        State_Update(_float fTimeDelta)             override;
    void        State_Late_Update(_float fTimeDelta)        override;
    void        State_Exit()                                override;

    void        Collision_Enter(CCollider* pOther)        override;
    void        Collision_Stay(CCollider* pOther)         override;
    void        Collision_Exit(CCollider* pOther)         override;

private:


public:
    static CState_Monster*  Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATE_SKELETON_DESC* pDesc);
    void   Free()    override;
};

END