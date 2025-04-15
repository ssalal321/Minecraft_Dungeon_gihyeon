#pragma once
#include "Client_Defines.h"
#include "State.h"
#include "Player.h"

BEGIN(Client)

class CState_Player abstract: public CState
{
protected:
    CState_Player(CGameObject* pActor, CModel* pPlayerModelCom, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
				  CTransform* pTransformCom, CNavigation* pNavigationCom);
	~CState_Player() override = default;

public:
    HRESULT   Init_State()          override;

    void   State_Enter()            override;
    void   State_Priority_Update(_float fTimeDelta)  override;
    void   State_Update(_float fTimeDelta)           override;
    void   State_Late_Update(_float fTimeDelta)      override;
    void   State_Exit()             override;

protected:
    CPlayer*                m_pPlayer = { nullptr };
    CPlayer::PLAYER_DESC*   m_pPlayerDesc = { nullptr };

    static  _bool			m_bAnimationFinished;
    _bool                   m_bClickLock = { false };

public:
    /*static CState*  Create(CGameObject* pActor, CGameObject* pPartObject, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
							CTransform* pTransformCom, CNavigation* pNavigationCom);*/
    void   Free()    override;
};

END