#pragma once
#include "Client_Defines.h"
#include "State_Slime_Small.h"

BEGIN(Client)
class CSlime_Small_Novelty final: public CState_Slime_Small
{
private:
    CSlime_Small_Novelty(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc);
	~CSlime_Small_Novelty() override = default;

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