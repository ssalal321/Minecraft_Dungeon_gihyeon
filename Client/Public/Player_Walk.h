#pragma once
#include "Client_Defines.h"
#include "State.h"
#include "Player.h"

BEGIN(Client)

class CPlayer_Walk final: public CState
{
private:
    CPlayer_Walk(CGameObject* pActor, CGameObject* pPartObject, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc);
	~CPlayer_Walk() override = default;

public:
    HRESULT   Init_State()          override;

    void   State_Enter()            override;
    void   State_Priority_Update(_float fTimeDelta)  override;
    void   State_Update(_float fTimeDelta)           override;
    void   State_Late_Update(_float fTimeDelta)      override;
    void   State_Exit()             override;

private:
    CPlayer*          m_pPlayer = { nullptr };
    class CBody_Player*     m_pBodyPlayer = { nullptr };
    CModel*                 m_pBodyPlayerModelCom = { nullptr };
    CPlayer::PLAYER_DESC*   m_pPlayerDesc = { nullptr };

public:
    static CState*  Create(CGameObject* pActor, CGameObject* pPartObject, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc);
    void   Free()    override;
};

END