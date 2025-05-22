#pragma once
#include "Client_Defines.h"
#include "State_CauldronBoss.h"

BEGIN(Client)

class CCauldronBoss_BasicAttack final: public CState_CauldronBoss
{
private:
    CCauldronBoss_BasicAttack(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc);
	~CCauldronBoss_BasicAttack() override = default;

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
    _bool       m_bAttackFinished = { false };
    _bool       m_bSummoned = { false };

private:
    void        Spawn_Monsters(const _float4& playerPos);

public:
    static CState_Monster*  Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc);
    void   Free()    override;
};

END