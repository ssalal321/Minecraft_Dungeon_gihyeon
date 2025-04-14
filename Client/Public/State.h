#pragma once
#include "Client_Defines.h"
#include "GameInstance.h"
#include "GameObject.h"

BEGIN(Engine)
	class CTransform;
END

BEGIN(Client)
class CState abstract : public CBase
{
protected:
    CState(CGameObject* pActor, CModel* pActorModelCom, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc,
			CTransform* pTransformCom, CNavigation* pNavigationCom)
        : m_pGameInstance(CGameInstance::GetInstance()),
		m_pActor(pActor), m_pActorModelCom(pActorModelCom), m_pGameObjectDesc(pGameObjectDesc),
		m_pTransformCom(pTransformCom), m_pNavigationCom(pNavigationCom)
    {
         Safe_AddRef(m_pGameInstance);
    }
	~CState() override = default;

public:
    virtual HRESULT   Init_State()          = 0;

    virtual void   State_Enter()            = 0;
    virtual void   State_Priority_Update(_float fTimeDelta)  = 0;
    virtual void   State_Update(_float fTimeDelta)           = 0;
    virtual void   State_Late_Update(_float fTimeDelta)      = 0;
    virtual void   State_Exit()             = 0;

    /*virtual void   On_CollisionEnter(GameEngine::Collision _other) {};
    virtual void   On_CollisionStay(GameEngine::Collision _other) {};
    virtual void   On_CollisionExit(GameEngine::Collision _other) {};*/

protected:
    CGameInstance*  m_pGameInstance     = { nullptr };
    CGameObject*    m_pActor            = { nullptr };
    CModel*         m_pActorModelCom    = { nullptr };
    CTransform*     m_pTransformCom     = { nullptr };
    CNavigation*    m_pNavigationCom    = { nullptr };
    CGameObject::GAMEOBJECT_DESC*    m_pGameObjectDesc   = { nullptr };

public:
    void    Free()  override;
};

END