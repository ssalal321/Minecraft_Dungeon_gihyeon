#pragma once
#include "Client_Defines.h"
#include "State_CauldronBoss.h"

BEGIN(Client)
	class CPlayer;

	class CCauldronBoss_StrongAttack final: public CState_CauldronBoss
{
private:
    CCauldronBoss_StrongAttack(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc);
	~CCauldronBoss_StrongAttack() override = default;

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
    CPlayer*    m_pPlayer = { nullptr };

    _bool       m_bShot = { false };

public:
    static CState_Monster*  Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc);
    void   Free()    override;
};

END