#pragma once
#include "Client_Defines.h"
#include "State_Slime_Cauldron.h"

BEGIN(Client)
	class CPlayer;

	class CSlime_Cauldron_Attack final: public CState_Slime_Cauldron
{
private:
	CSlime_Cauldron_Attack(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATE_SLIME_CAULDRON_DESC* pDesc);
	~CSlime_Cauldron_Attack() override = default;

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
    CPlayer*    m_pPlayer = { nullptr };

    _bool       m_bShot = { false };

public:
    static CState_Monster*  Create(CGameObject* pActor, CGameObject::GAMEOBJECT_DESC* pGameObjectDesc, STATE_SLIME_CAULDRON_DESC* pDesc);
    void   Free()    override;
};

END