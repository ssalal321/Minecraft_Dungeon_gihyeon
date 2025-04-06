#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CPlayer_Idle final: public CState
{
private:
	CPlayer_Idle(CGameObject* pActor, CGameObject* pPartObject, OBJECT_DESC* pGameObjectDesc);
	~CPlayer_Idle() override = default;

public:
    HRESULT   Init_State()          override;

    void   State_Enter()            override;
    void   State_Priority_Update()  override;
    void   State_Update()           override;
    void   State_Late_Update()      override;
    void   State_Exit()             override;

private:
    class CPlayer*          m_pPlayer = { nullptr };
    class CBody_Player*     m_pBodyPlayer = { nullptr };
    CModel*                 m_pBodyPlayerModelCom = { nullptr };
    PLAYER_DESC*            m_pPlayerDesc = { nullptr };

public:
    static CState*  Create(CGameObject* pActor, CGameObject* pPartObject, OBJECT_DESC* pGameObjectDesc);
    void   Free()    override;
};

END