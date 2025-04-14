#pragma once
#include "Client_Defines.h"
#include "State_Zombie.h"

BEGIN(Client)
class CZombie_Sleep final: public CState_Zombie
{
private:
    CZombie_Sleep(CGameObject* pActor, CModel* pZombieModelCom, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
				 CTransform* pTransformCom, CNavigation* pNavigationCom);
	~CZombie_Sleep() override = default;

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