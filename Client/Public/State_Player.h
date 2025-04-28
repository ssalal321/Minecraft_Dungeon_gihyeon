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
        CArrowPool_Player*  pArrowPool_Player = { nullptr };

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
    CPlayer::PLAYER_DESC*   m_pPlayerInfo = { nullptr };
    STATEPLAYER_DESC*       m_pStatePlayerDesc = { nullptr };
    CCollider*              m_pColliderOBBCom = { nullptr };
    CArrowPool_Player*      m_pArrowPool_Player = { nullptr };

    _bool			        m_bAnimationFinished = { false };
    //_bool                   m_bClickLock = { false };

    static _bool            m_bCombo1_Finished;
    static _bool            m_bCombo2_Finished;
    static _bool            m_bCombo3_Finished;
    static _bool            m_bComboInitiating;
    static _float           m_fCombo_ElapsedTime;

    _float                  m_fPrevAnimTrackPosition = {};

protected:
    _bool   Change_State_To_Idle();
    _bool   Change_State_To_Walk();
    _bool   Change_State_To_Roll();
    _bool   Change_State_To_GlaiveCombo();   // 최대한 Walk에서만 호출하기
    _bool   Change_State_To_BowAction();
    _bool   Change_State_To_GetHitFront(CCollider* pOther);

    void    Check_Combo_Timeout(_float fTimeDelta);
    void    Reset_Combo();

public:
    void   Free()    override;
};

END