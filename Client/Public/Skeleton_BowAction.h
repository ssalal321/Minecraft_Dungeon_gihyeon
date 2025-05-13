#pragma once
#include "Client_Defines.h"
#include "State_Skeleton.h"

BEGIN(Client)

class CSkeleton_BowAction final: public CState_Skeleton
{
private:
    CSkeleton_BowAction(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATE_SKELETON_DESC* pDesc);
	~CSkeleton_BowAction() override = default;

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
	_bool       m_bShotArrow = { false };

    _float4     m_PlayerPosition = { 0.f, 0.f, 0.f, 1.f };

private:
    void        Shoot_Arrow();

public:
    static CState_Monster*  Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATE_SKELETON_DESC* pDesc);
    void   Free()    override;
};

END