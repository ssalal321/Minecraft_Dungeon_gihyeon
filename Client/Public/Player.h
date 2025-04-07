#pragma once
#include "Client_Defines.h"
#include "ContainerObject.h"

/* 플레이어라는 객체를 구성하기위한 파츠들을 모아서 쥐고 있는 객체. */

BEGIN(Client)
class CState;

class CPlayer final : public CContainerObject
{
public:
	struct PLAYER_DESC : public GAMEOBJECT_DESC
	{
		_int     iCurrentHP;
		_int     iMaxHP;
		_int     iAttackPoint;
		_float   fEffectiveRange;
		_bool    bStunned;

		PLAYER_DESC(const _tchar* GameObjectTag, _int currentHP, _int maxHP, _int attackPoint,
			_float effectiveRange, _bool stunned = false,
			_float rotationPerSec = 0.f, _float speedPerSec = 0.f)
			: GAMEOBJECT_DESC(GameObjectTag, rotationPerSec, speedPerSec), iCurrentHP(currentHP), iMaxHP(maxHP), iAttackPoint(attackPoint),
			fEffectiveRange(effectiveRange), bStunned(stunned) {
		}

		~PLAYER_DESC() override = default;
	};

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

	const _float4&	Get_NextPosition() const { return m_NextPosition; }

	void	Set_NextPosition(const _float4& nextPosition)
	{
		m_NextPosition = nextPosition;
	}

	void	Change_State(PLAYER_STATE playerState);
	

private:
	_uint				m_iState = { STATE_END };
	class FSM*			m_pPlayerFSM = { nullptr };
	PLAYER_DESC*		m_pPlayerInfo = { nullptr };
	vector<CState*>     m_StatesVec;

	_float4				m_NextPosition = {0.f, 0.f, 0.f, 1.f};

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