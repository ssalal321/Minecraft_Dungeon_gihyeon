#pragma once
#include "Client_Defines.h"
#include "ContainerObject.h"

/* 플레이어라는 객체를 구성하기위한 파츠들을 모아서 쥐고 있는 객체. */

BEGIN(Client)
class CState;

class CPlayer final : public CContainerObject
{
public:
	/*enum PLAYERSTATE
	{
		IDLE, IDLE_CLAYMORE, IDLE_GLAIVE, IDLE_HAMMER, IDLE_KATANA, IDLE_LOOKAROUND,
		WALK, WALK_CLAYMORE, WALK_GLAIVE, WALK_HAMMER, WALK_KATANA,
		RUN,  RUN_CLAYMORE,  RUN_GLAIVE,  RUN_HAMMER,  RUN_KATANA,
		BOWACTION, DODGEROLL, STUN, FALLING, GETHITFRONT, DOWNEDFLOOR,
		DRINK, EATFAST, HORNBLOW, LASERPOSE,
		TELEPORTINITIATE, TELEPORTLANDING, ITEMONGROUND,
		STATE_END
	};*/

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	~CPlayer() override = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

public:
	vector<CState*>   Get_StateVec() { return m_StatesVec; }

	void	Change_State(class CState* _nextState);
	

private:
	_uint				m_iState = { STATE_END };
	class FSM*			m_pPlayerFSM = { nullptr };
	PLAYER_DESC*		m_pPlayerInfo = { nullptr };
	vector<CState*>     m_StatesVec;

private:
	HRESULT		Ready_Components();
	HRESULT		Ready_PartObjects();
	HRESULT		Ready_States();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)	override;
	void	Free()						override;
};

END