#pragma once
#include "Client_Defines.h"
#include "State_Slime_Medium.h"

BEGIN(Client)

class CSlime_Medium_Attack final: public CState_Slime_Medium
{
private:
    CSlime_Medium_Attack(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc);
	~CSlime_Medium_Attack() override = default;

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
    _float      m_fCoolTime = 0.f;
    _bool       m_bHitMode_Activated = false;

public:
    static CState_Monster*  Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc);
    void   Free()    override;
};

END