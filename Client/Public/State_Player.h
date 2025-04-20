#pragma once
#include "Client_Defines.h"
#include "State.h"
#include "Player.h"

BEGIN(Client)

class CState_Player abstract: public CState
{
public:
    typedef struct tagStatePlayer
    {
        CModel*         pActorModelCom = { nullptr };
        CCollider*      pColliderOBBCom = { nullptr };
        CTransform*     pTransformCom = { nullptr };
        CNavigation*    pNavigationCom = { nullptr };

    }STATEPLAYER_DESC;

protected:
    CState_Player(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEPLAYER_DESC* pDesc);
	~CState_Player() override = default;

public:
    HRESULT     Init_State()                                override;

    void        State_Enter()                               override;
    void        State_Priority_Update(_float fTimeDelta)    override;
    void        State_Update(_float fTimeDelta)             override;
    void        State_Late_Update(_float fTimeDelta)        override;
    void        State_Exit()                                override;

    void        Collision_Enter(CCollider* pOther)          override;
    void        Collision_Stay(CCollider* pOther)           override;
    void        Collision_Exit(CCollider* pOther)           override;

protected:
    CPlayer*                m_pPlayer = { nullptr };
    CPlayer::PLAYER_DESC*   m_pPlayerDesc = { nullptr };
    STATEPLAYER_DESC*       m_pStatePlayerDesc = { nullptr };
    CCollider*              m_pColliderOBBCom = { nullptr };

    _bool			        m_bAnimationFinished = { false };
    _bool                   m_bClickLock = { false };
    _bool                   m_bCombo1_Finished = { false };
    _bool                   m_bCombo2_Finished = { false };
    _bool                   m_bCombo3_Finished = { false };
    _float                  m_fPrevAnimTrackPosition = {};

protected:
    _bool   Change_State_To_Idle();
    _bool   Change_State_To_Walk();
    _bool   Change_State_To_Roll();
    _bool   Change_State_To_Attack();

public:
    void   Free()    override;
};

END