#pragma once
#include "Client_Defines.h"
#include "State_Vindicator.h"

BEGIN(Client)

class CVindicator_Walk final: public CState_Vindicator
{
private:
    CVindicator_Walk(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc);
	~CVindicator_Walk() override = default;

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
    //_bool       m_bRetreatingState = { false };
    _vector     m_vRetreatDir = { 0,0,0,0 };
    _float4     m_vRetreatStartPos = { 0.f,0.f,0.f,1.f };

public:
    static CState_Monster*  Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc);
    void   Free()    override;
};

END