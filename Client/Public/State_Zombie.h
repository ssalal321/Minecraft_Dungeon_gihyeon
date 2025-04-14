#pragma once
#include "Client_Defines.h"
#include "State_Monster.h"

BEGIN(Client)
	class CZombie;

	class CState_Zombie abstract: public CState_Monster
{
protected:
    CState_Zombie(CGameObject* pActor, CModel* pMonsterModelCom, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
				   CTransform* pTransformCom, CNavigation* pNavigationCom);
	~CState_Zombie() override = default;

public:
    HRESULT   Init_State()                              override;

    void      State_Enter()                             override;
    void      State_Priority_Update(_float fTimeDelta)  override;
    void      State_Update(_float fTimeDelta)           override;
    void      State_Late_Update(_float fTimeDelta)      override;
    void      State_Exit()                              override;

protected:
    CZombie*    m_pZombie = { nullptr };

public:
    void   Free()    override;
};

END