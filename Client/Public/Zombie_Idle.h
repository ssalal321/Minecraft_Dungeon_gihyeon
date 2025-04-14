#pragma once
#include "Client_Defines.h"
#include "State_Monster.h"

BEGIN(Client)
	class CZombie;

	class CZombie_Idle final: public CState_Monster
{
private:
	CZombie_Idle(CGameObject* pActor, CModel* pZombieModelCom, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
				 CTransform* pTransformCom, CNavigation* pNavigationCom);
	~CZombie_Idle() override = default;

public:
    HRESULT     Init_State()                                override;

    void        State_Enter()                               override;
    void        State_Priority_Update(_float fTimeDelta)    override;
    void        State_Update(_float fTimeDelta)             override;
    void        State_Late_Update(_float fTimeDelta)        override;
    void        State_Exit()                                override;

private:
    CZombie*    m_pZombie = { nullptr };

public:
    static CState_Monster*  Create(CGameObject* pActor, CModel* pZombieModelCom, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
								   CTransform* pTransformCom, CNavigation* pNavigationCom);
    void   Free()    override;
};

END