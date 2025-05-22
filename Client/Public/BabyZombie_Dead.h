#pragma once
#include "Client_Defines.h"
#include "State_BabyZombie.h"

BEGIN(Client)

class CBabyZombie_Dead final: public CState_BabyZombie
{
private:
    CBabyZombie_Dead(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc);
	~CBabyZombie_Dead() override = default;

public:
    HRESULT     Init_State()                                override;

    void        State_Enter()                               override;
    void        State_Priority_Update(_float fTimeDelta)    override;
    void        State_Update(_float fTimeDelta)             override;
    void        State_Late_Update(_float fTimeDelta)        override;
    void        State_Exit()                                override;

    void        Collision_Enter(CCollider* pOther)        override;
    void        Collision_Stay(CCollider* pOther)         override;
    void        Collision_Exit(CCollider* pOther)         override;

private:
    _bool       m_bDowned = { false };
    _bool       m_bDelayFinished = { false };
	_float      m_fAccumulatedRotation = {};
    _float      m_fDelayTime = {};

    _float      m_fCurrentTime  = {};  // 초기화
    _float      m_fDurationTime = {}; // 애니메이션 지속 시간 (예: 3초)

    _vector     m_vFallAxis = {};

    _bool        m_bCurveFinished = { false };

public:
    static CState_Monster*  Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATEMONSTER_DESC* pDesc);
    void   Free()    override;
};

END