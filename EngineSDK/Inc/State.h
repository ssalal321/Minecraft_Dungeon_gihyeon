#pragma once
#include "PartObject.h"

BEGIN(Engine)

class ENGINE_DLL CState abstract : public CBase
{
protected:
    CState(CPartObject * pActor, CTransform* pTransformCom) : m_pActor(pActor), m_pTransformCom(pTransformCom) {}
    ~CState() override = default;

public:
    virtual void   Init_State()             = 0;

    virtual void   State_Enter()            = 0;
    virtual void   State_Priority_Update()  = 0;
    virtual void   State_Update()           = 0;
    virtual void   State_Late_Update()      = 0;
    virtual void   State_Exit()             = 0;

    /*virtual void   On_CollisionEnter(GameEngine::Collision _other) {};
    virtual void   On_CollisionStay(GameEngine::Collision _other) {};
    virtual void   On_CollisionExit(GameEngine::Collision _other) {};*/

protected:
    class CPartObject*    m_pActor;
    class CTransform*     m_pTransformCom;

public:
    void    Free()  override = 0;
};

END