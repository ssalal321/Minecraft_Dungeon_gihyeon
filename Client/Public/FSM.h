#pragma once
#include "Base.h"
#include "State.h"

BEGIN(Client)

class FSM final : public CBase
{
public:
	FSM() {}
	~FSM() override = default;

public:
	void Init_State(CState* pInitState)
	{
		if (m_curState == nullptr)
		{
			m_curState = pInitState;
			m_curState->State_Enter();
		}
	}

	void Change_State(CState* pNextState)
	{
		if (pNextState == nullptr)
			return;

		if (pNextState == m_curState)
			return;

		if (m_curState != nullptr)
			m_curState->State_Exit();

		m_curState = pNextState;
		m_curState->State_Enter();
	}

	void Priority_Update_State(_float fTimeDelta)
	{
		if (m_curState != nullptr)
			m_curState->State_Priority_Update(fTimeDelta);
	}

	void Update_State(_float fTimeDelta)
	{
		if (m_curState != nullptr)
			m_curState->State_Update(fTimeDelta);
	}

	void Late_Update_State(_float fTimeDelta)
	{
		if (m_curState != nullptr)
			m_curState->State_Late_Update(fTimeDelta);
	}

	void Collision_Enter(CCollider* pOther)
	{
		if (m_curState != nullptr)
			m_curState->Collision_Enter(pOther);
	}

	void Collision_Stay(CCollider* pOther)
	{
		if (m_curState != nullptr)
			m_curState->Collision_Stay(pOther);
	}

	void Collision_Exit(CCollider* pOther)
	{
		if (m_curState != nullptr)
			m_curState->Collision_Exit(pOther);
	}

	CState*  Get_CurrentState() const { return m_curState; }

private:
	CState*		m_curState = { nullptr };

public:
	static FSM* Create()
	{
		FSM* pGameInstance = new FSM();

		if (nullptr == pGameInstance)
		{
			MSG_BOX("Failed to Create : FSM");
			Safe_Release(pGameInstance);
		}

		return pGameInstance;
	}

	void Free() override
	{
	}
};

END