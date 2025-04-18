#pragma once
#include "Client_Defines.h"
#include "State_Zombie.h"

BEGIN(Client)

class CZombie_Attack final: public CState_Zombie
{
private:
    CZombie_Attack(CGameObject* pActor, CModel* pZombieModelCom, CCollider* pColliderCom,
                    CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
                    CTransform* pTransformCom, CNavigation* pNavigationCom);
	~CZombie_Attack() override = default;

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
    _float      m_fAnimTimer = 0.f;
    _bool       m_bHitbox_Activated = false;

public:
    static CState_Monster*  Create(CGameObject* pActor, CModel* pZombieModelCom, CCollider* pColliderCom,
                                    CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
                                    CTransform* pTransformCom, CNavigation* pNavigationCom);
    void   Free()    override;
};

END