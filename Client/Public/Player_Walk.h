#pragma once
#include "Client_Defines.h"
#include "State_Player.h"

BEGIN(Client)
	class CPlayer_Walk final: public CState_Player
{
private:
    CPlayer_Walk(CGameObject* pActor, CGameObject* pPartObject, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
				 CTransform* pTransformCom, CNavigation* pNavigationCom);
	~CPlayer_Walk() override = default;

public:
    HRESULT     Init_State()                                override;

    void        State_Enter()                               override;
    void        State_Priority_Update(_float fTimeDelta)    override;
    void        State_Update(_float fTimeDelta)             override;
    void        State_Late_Update(_float fTimeDelta)        override;
    void        State_Exit()                                override;

private:


public:
    static CState_Player*  Create(CGameObject* pActor, CGameObject* pPartObject, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
								  CTransform* pTransformCom, CNavigation* pNavigationCom);
    void   Free()    override;
};

END