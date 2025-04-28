#pragma once
#include "Client_Defines.h"
#include "ContainerObject.h"

/* 플레이어라는 객체를 구성하기위한 파츠들을 모아서 쥐고 있는 객체. */

BEGIN(Client)
class CState;
class CInventoryData;
class CArrowPool_Player;

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

		_int	 iArrowNum;

		PLAYER_DESC(const _wstring& GameObjectTag, _int currentHP, const _int& maxHP, _int attackPoint,
			const _float&  effectiveRange, _int arrowNum, _bool stunned = false,
			_float rotationPerSec = 0.f, _float speedPerSec = 0.f)
			: GAMEOBJECT_DESC(GameObjectTag, rotationPerSec, speedPerSec), iCurrentHP(currentHP), iMaxHP(maxHP), iAttackPoint(attackPoint),
			fEffectiveRange(effectiveRange), bStunned(stunned), iArrowNum(arrowNum) {
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
	HRESULT		Initialize(void* pArg = nullptr)	override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

public:
	const _float4&	Get_NextPosition() const { return m_NextPosition; }
	const _bool&	Get_Chasing() const { return m_bChasing; }
	CTransform*		Get_MonsterTransformCom() const { return m_pMonsterTransformCom; }
	const _bool&	Get_ShootArrow() const { return m_bShootArrow; }
	const _float4&	Get_PickedPosition() const { return m_PickedPos; }
	const _bool&	Get_Attacking() const { return m_bAttacking; }

	void	Set_Attacking(_bool bAttacking) { m_bAttacking = bAttacking; }

	void	Set_NextPosition(const _float4& nextPosition)
	{
		m_NextPosition = nextPosition;
	}

	void	Set_Chasing(_bool bChasing, CTransform* pMonsterTransformCom)
	{
		m_bChasing = bChasing;
		m_pMonsterTransformCom = pMonsterTransformCom;
	}

	void	Set_Shoot_Arrow(_bool bShootArrow, _float4 pickedPos)
	{
		m_bShootArrow = bShootArrow;
		m_PickedPos = pickedPos;
	}

	void	Set_Shoot_Arrow(_bool bShootArrow) { m_bShootArrow = bShootArrow; }

public:
	void	Change_State(PLAYER_STATE playerState);

	void	Collided_With(CCollider* pOther, CCollider::COLLISION_STATE eCollisionState) override;

private:
	PLAYER_DESC*		m_pPlayerInfo = { nullptr };
	_float4				m_NextPosition = { 0.f, 0.f, 0.f, 1.f };

	CNavigation*		m_pNavigationCom = { nullptr };

	CArrowPool_Player*	m_pArrowPool_Player = { nullptr };

	CInventoryData*		m_pInventoryData = { nullptr };

#pragma region FSM
	_uint				m_iState	  = { static_cast<_uint>(PLAYER_STATE::STATE_END) };
	class FSM*			m_pPlayerFSM  = { nullptr };
	vector<CState*>     m_StatesVec;
#pragma endregion

#pragma region MONSTER
	_bool				m_bAttacking	= { false };
	_bool				m_bChasing		= { false };
	CTransform*			m_pMonsterTransformCom = { nullptr };

	_bool				m_bShootArrow	= { false };
	_float4				m_PickedPos	= { 0.f, 0.f, 0.f, 1.f };
#pragma endregion

private:
	HRESULT		Ready_Components();
	HRESULT		Ready_Inventory();
	HRESULT		Ready_PartObjects();
	HRESULT		Ready_States();


public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)	override;
	void	Free()						override;
};

END