#pragma once
#include "Client_Defines.h"
#include "State_Player.h"

BEGIN(Client)
	class CPlayer_Roll final: public CState_Player
{
private:
    CPlayer_Roll(CGameObject* pActor, CGameObject* pPartObject, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
				 CTransform* pTransformCom, CNavigation* pNavigationCom);
	~CPlayer_Roll() override = default;

public:
    HRESULT     Init_State()                                override;

    void        State_Enter()                               override;
    void        State_Priority_Update(_float fTimeDelta)    override;
    void        State_Update(_float fTimeDelta)             override;
    void        State_Late_Update(_float fTimeDelta)        override;
    void        State_Exit()                                override;

private:
    _float      m_fRollingTime = { 0.f };

public:
    static CState_Player*  Create(CGameObject* pActor, CGameObject* pPartObject, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
								  CTransform* pTransformCom, CNavigation* pNavigationCom);
    void   Free()    override;
};

END