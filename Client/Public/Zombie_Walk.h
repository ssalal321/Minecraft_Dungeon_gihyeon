#pragma once
#include "Client_Defines.h"
#include "State_Zombie.h"

BEGIN(Client)

class CZombie_Walk final: public CState_Zombie
{
private:
    CZombie_Walk(CGameObject* pActor, CModel* pZombieModelCom, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
				 CTransform* pTransformCom, CNavigation* pNavigationCom);
	~CZombie_Walk() override = default;

public:
    HRESULT     Init_State()                                override;

    void        State_Enter()                               override;
    void        State_Priority_Update(_float fTimeDelta)    override;
    void        State_Update(_float fTimeDelta)             override;
    void        State_Late_Update(_float fTimeDelta)        override;
    void        State_Exit()                                override;

private:


public:
    static CState_Monster*  Create(CGameObject* pActor, CModel* pZombieModelCom, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
								   CTransform* pTransformCom, CNavigation* pNavigationCom);
    void   Free()    override;
};

END