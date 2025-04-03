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
	void Init_State(CState* _pInitState)
	{
		if (m_curState == nullptr)
		{
			m_curState = _pInitState;
			m_curState->State_Enter();
		}
	}

	void Change_State(CState* _pNextState)
	{
		if (_pNextState == nullptr)
			return;

		if (_pNextState == m_curState)
			return;

		if (m_curState != nullptr)
			m_curState->State_Exit();

		m_curState = _pNextState;
		m_curState->State_Enter();
	}

	void Priority_Update_State()
	{
		if (m_curState != nullptr)
			m_curState->State_Priority_Update();
	}

	void Update_State()
	{
		if (m_curState != nullptr)
			m_curState->State_Update();
	}

	void Late_Update_State()
	{
		if (m_curState != nullptr)
			m_curState->State_Late_Update();
	}

	/*void Collision_Enter(GameEngine::Collision _other)
	{
		if (m_curState != nullptr)
			m_curState->On_CollisionEnter(_other);
	}

	void Collision_Stay(GameEngine::Collision _other)
	{
		if (m_curState != nullptr)
			m_curState->On_CollisionStay(_other);
	}

	void Collision_Exit(GameEngine::Collision _other)
	{
		if (m_curState != nullptr)
			m_curState->On_CollisionExit(_other);
	}*/

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

	void Free()
	{
	}
};

END