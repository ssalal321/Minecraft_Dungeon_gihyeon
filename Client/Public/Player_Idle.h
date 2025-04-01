#pragma once
#include "State.h"

BEGIN(Client)

class CPlayer_Idle final: public CState
{
private:
	CPlayer_Idle(CPartObject* pActor, CTransform* pTransformCom);
	~CPlayer_Idle() override = default;

public:
    void   Init_State()             override;

    void   State_Enter()            override;
    void   State_Priority_Update()  override;
    void   State_Update()           override;
    void   State_Late_Update()      override;
    void   State_Exit()             override;

public:
    void   Free()                   override;
};

END