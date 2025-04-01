#pragma once

#include "State.h"

BEGIN(Engine)

class ENGINE_DLL FSM : public CBase
{
public:
	FSM() : m_curState(nullptr) {}
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

	void Fixed_Update_State()
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
	CState*		m_curState;
	//unordered_map<int, vector<CState*>>	  m_StateMap;

public:
	static	FSM*	Create();
	void   Free()    override;
};

END