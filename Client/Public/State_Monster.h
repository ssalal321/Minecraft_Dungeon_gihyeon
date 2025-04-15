#pragma once
#include "Client_Defines.h"
#include "State.h"
#include "Monster.h"

BEGIN(Client)

class CState_Monster abstract: public CState
{
protected:
    CState_Monster(CGameObject* pActor, CModel* pMonsterModelCom, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
				   CTransform* pTransformCom, CNavigation* pNavigationCom);
	~CState_Monster() override = default;

public:
    HRESULT   Init_State()                              override;

    void      State_Enter()                             override;
    void      State_Priority_Update(_float fTimeDelta)  override;
    void      State_Update(_float fTimeDelta)           override;
    void      State_Late_Update(_float fTimeDelta)      override;
    void      State_Exit()                              override;

protected:
    CMonster::MONSTER_DESC*     m_pMonsterDesc = { nullptr };

    /*static  */_bool			m_bAnimationFinished = false;

public:
    void   Free()    override;
};

END