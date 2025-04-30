#pragma once
#include "Client_Defines.h"
#include "State.h"
#include "Monster.h"

BEGIN(Client)

class CState_Monster abstract: public CState
{
public:
    typedef struct tagStateMonster
    {
        CModel*         pActorModelCom      = { nullptr };
        CCollider*      pColliderCom     = { nullptr };
        //CCollider*      pColliderSphereCom  = { nullptr };
        CTransform*     pTransformCom       = { nullptr };
        CNavigation*    pNavigationCom      = { nullptr };

    }STATEMONSTER_DESC;

protected:
    CState_Monster(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc);
	~CState_Monster() override = default;

public:
    HRESULT   Init_State()                                override;

    void      State_Enter()                               override;
    void      State_Priority_Update(_float fTimeDelta)    override;
    void      State_Update(_float fTimeDelta)             override;
    void      State_Late_Update(_float fTimeDelta)        override;
    void      State_Exit()                                override;

    void      Collision_Enter(CCollider* pOther)        override;
    void      Collision_Stay(CCollider* pOther)         override;
    void      Collision_Exit(CCollider* pOther)         override;

protected:
    CMonster::MONSTER_DESC*     m_pMonsterInfo = { nullptr };
    STATEMONSTER_DESC*          m_pStateMonsterDesc = { nullptr };
    CCollider*                  m_pColliderCom = { nullptr };

    CCollider*  m_pColliderSphereCom = { nullptr };

    _bool	m_bAnimationFinished = { false };

public:
    void   Free()    override;
};

END