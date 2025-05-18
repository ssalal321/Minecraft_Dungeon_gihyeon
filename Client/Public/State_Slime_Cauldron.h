#pragma once
#include "Client_Defines.h"
#include "State_Monster.h"

BEGIN(Client)
	class CBulletPool_Monster;
	class CSlime_Cauldron;

class CState_Slime_Cauldron abstract: public CState_Monster
{
public:
    typedef struct tagStateSlimeCauldron : STATEMONSTER_DESC
    {
        CBulletPool_Monster*    pBulletPool_Monster = { nullptr };

    }STATE_SLIME_CAULDRON_DESC;

protected:
    CState_Slime_Cauldron(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATE_SLIME_CAULDRON_DESC* pDesc);
	~CState_Slime_Cauldron() override = default;

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
    CSlime_Cauldron*    m_pSlime_Cauldron = { nullptr };

    CBulletPool_Monster* m_pBulletPool_Monster = { nullptr };

protected:
    _bool     Modify_HP(CCollider* pOther);

public:
    void   Free()    override;
};

END