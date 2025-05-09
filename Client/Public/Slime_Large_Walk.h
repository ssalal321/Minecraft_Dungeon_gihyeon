#pragma once
#include "Client_Defines.h"
#include "State_Slime_Large.h"

BEGIN(Client)

class CSlime_Large_Walk final: public CState_Slime_Large
{
private:
    CSlime_Large_Walk(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc);
	~CSlime_Large_Walk() override = default;

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
    _vector     m_vWalkStartPos  = XMVectorZero();
    _vector     m_vRandomWalkDir = XMVectorZero();
    _bool       m_bLostPlayer = { false };
    _float      m_fDistance = {};

private:
    void    Direction_Setting();

public:
    static CState_Monster*  Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc);
    void   Free()    override;
};

END