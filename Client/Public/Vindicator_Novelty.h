#pragma once
#include "Client_Defines.h"
#include "State_Vindicator.h"

BEGIN(Client)
class CVindicator_Novelty final: public CState_Vindicator
{
private:
    CVindicator_Novelty(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc);
	~CVindicator_Novelty() override = default;

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
    static CState_Monster*  Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc);
    void   Free()    override;
};

END