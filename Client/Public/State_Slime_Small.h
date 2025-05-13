#pragma once
#include "Client_Defines.h"
#include "State_Monster.h"

BEGIN(Client)
class CSlime_Small;

class CState_Slime_Small abstract: public CState_Monster
{
protected:
    CState_Slime_Small(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc);
	~CState_Slime_Small() override = default;

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
    CSlime_Small*    m_pSlime_Small = { nullptr };

protected:
    _bool     Change_State_To_Attack();
    _bool     Change_State_To_Walk();
    _bool     Change_State_To_Idle();
    _bool     Modify_HP(CCollider* pOther);
    _bool     Change_State_To_Stun(CCollider* pOther);

public:
    void   Free()    override;
};

END