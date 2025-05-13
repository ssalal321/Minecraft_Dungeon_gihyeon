#pragma once
#include "Client_Defines.h"
#include "State_Player.h"

BEGIN(Client)

class CPlayer_BowAction final : public CState_Player
{
private:
    CPlayer_BowAction(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEPLAYER_DESC* pDesc);
    ~CPlayer_BowAction() override = default;

public:
    HRESULT     Init_State()                                override;

    void        State_Enter()                               override;
    void        State_Priority_Update(_float fTimeDelta)    override;
    void        State_Update(_float fTimeDelta)             override;
    void        State_Late_Update(_float fTimeDelta)        override;
    void        State_Exit()                                override;

    void        Collision_Enter(CCollider* pOther)          override;
    void        Collision_Stay(CCollider* pOther)           override;
    void        Collision_Exit(CCollider* pOther)           override;

private:
    _bool       m_bShotArrow = { false };

private:
    void        Shoot_Arrow();

public:
    static CState_Player*  Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEPLAYER_DESC* pDesc);
    void   Free()    override;
};

END