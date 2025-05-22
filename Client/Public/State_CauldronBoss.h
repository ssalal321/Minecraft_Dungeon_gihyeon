#pragma once
#include "Client_Defines.h"
#include "State_Monster.h"

BEGIN(Client)
class CCauldronBoss;

class CState_CauldronBoss abstract: public CState_Monster
{
protected:
    CState_CauldronBoss(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc);
	~CState_CauldronBoss() override = default;

public:
    HRESULT   Init_State()                              override;

    void      State_Enter()                             override;
    void      State_Priority_Update(_float fTimeDelta)  override;
    void      State_Update(_float fTimeDelta)           override;
    void      State_Late_Update(_float fTimeDelta)      override;
    void      State_Exit()                              override;

    void      Collision_Enter(CCollider* pOther)      override;
    void      Collision_Stay(CCollider* pOther)       override;
    void      Collision_Exit(CCollider* pOther)       override;

protected:
    CCauldronBoss*    m_pCauldronBoss = { nullptr };

protected:
    _bool     Change_State_To_BasicAttack();
    _bool     Change_State_To_StrongAttack();
    _bool     Change_State_To_IntenseSpin(CCollider* pOther);
    _bool     Change_State_To_Idle();
    _bool     Modify_HP(CCollider* pOther);

public:
    void   Free()    override;
};

END