#pragma once
#include "Client_Defines.h"
#include "ContainerObject.h"

BEGIN(Engine)
class	CShader;
class	CModel;
END

BEGIN(Client)
	class CArrowPool_Monster;
	class CState;

class CMonster abstract : public CContainerObject
{
public:
	struct MONSTER_DESC : public GAMEOBJECT_DESC
	{
		_int      iCurrentHP;
		_int      iMaxHP;
		_int      iDealPoint;
		_float    fAttackableRange;
		_float	  fDetectableRange;
		_bool     bStunned;

		MONSTER_DESC(const _wstring& gameObjectTag, _int currentHP, const _int& maxHP, _int dealPoint,
			_float attackableRange, _float detectRange, _bool stunned = false,
			_float rotationPerSec = 0.f, _float speedPerSec = 0.f)
			: GAMEOBJECT_DESC(gameObjectTag, rotationPerSec, speedPerSec), iCurrentHP(currentHP), iMaxHP(maxHP), iDealPoint(dealPoint),
			fAttackableRange(attackableRange), fDetectableRange(detectRange), bStunned(stunned) {
		}

		~MONSTER_DESC() override = default;

		const _int&		Get_CurrentHP()			const { return iCurrentHP; }
		const _int&		Get_MaxHP()				const { return iMaxHP; }
		const _int&		Get_DealPoint()		const { return iDealPoint; }
		const _float&	Get_AttackRange()	const { return fAttackableRange; }
		const _float&	Get_DetectRange()	const { return fDetectableRange; }

		void Modify_CurrentHp(_int iDamageOrHeal)
		{
			_int iModifiedHP = iCurrentHP + iDamageOrHeal;

			if (iModifiedHP <= 0)
			{
				iCurrentHP = 0;
				return;
			}

			if (iModifiedHP >= iMaxHP)
			{
				iCurrentHP = iMaxHP;
				return;
			}

			iCurrentHP = iModifiedHP;
		}

	};

protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& Prototype);
	~CMonster() override = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg = nullptr)	override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

public:
	MONSTER_DESC*  Get_MonsterInfo() { return m_pMonsterInfo; }

	_bool	Get_Attacking() const { return m_bAttacking; }

	const _float4& Get_NextPosition() const { return m_NextPosition; }

	_bool		Get_Can_be_Eaten() { return m_bCanbeEaten; }
	CMonster*	Get_Eating_BossMonster() { return m_pBossMonster; }

	_bool		Get_Is_Jumping() const { return m_bIsJumping; }
	_vector		Get_Jump_LandPos() const { return m_vJumpTarget; }

	void		Set_Is_Jumping(_bool bJumping) { m_bIsJumping = bJumping; }


	void	Set_Attacking(_bool bAttacking) { m_bAttacking = bAttacking; }

	void	Set_NextPosition(const _float4& nextPosition)
	{
		m_NextPosition = nextPosition;
	}

	void	Set_Hovered(_bool bPicked) { m_bHovered = bPicked; }
	void	Set_Can_be_Eaten(_bool bCanbeEaten, CMonster* pBossMonster)
	{
		m_bCanbeEaten = bCanbeEaten;
		m_pBossMonster = pBossMonster;
	}

	void	Jump_To_Target(_vector vTargetPos)
	{
		m_bIsJumping = true;
		m_vJumpTarget = vTargetPos;
	}

	void		Change_State(const MonsterState& state);
	void		Collided_With(CCollider* pOther, CCollider::COLLISION_STATE eCollisionState)	override;

	_float4		Get_Player_Position(const _wstring& strPlayerGameObjectTag, _uint iPlayerLayerLevelIndex) const;
	_vector		Vec_To_Player(const _wstring& strPlayerGameObjectTag, _uint iPlayerLayerLevelIndex) const;
	_float		Length_To_Player() const;
	_bool		Player_In_DetectRange() const;

	void		Render_DamageFont(_int iDealPoint, _float fStartY);

protected:
	_uint				m_iState = { static_cast<_uint>(ZOMBIE_STATE::STATE_END) };
	class FSM*			m_pMonsterFSM = { nullptr };
	MONSTER_DESC*		m_pMonsterInfo = { nullptr };
	vector<CState*>     m_StatesVec;

	CNavigation*		m_pNavigationCom = { nullptr };

	_float4				m_NextPosition = { 0.f, 0.f, 0.f, 1.f };
	_bool				m_bAttacking = { false };
	_bool				m_bAlwaysActivated = { true };
	
	_bool				m_bHovered = { false };
	_bool				m_bCanbeEaten = { false };
	CMonster*			m_pBossMonster = { nullptr };

	_bool				m_bIsJumping = { false };
	_vector				m_vJumpTarget = {};

#pragma region DAMAGE_FONT
	_bool				m_bRenderDamageFont = { false };
	_int				m_iDealPoint = {};
	_float				m_fFontRenderedTime = {};

	_float2				m_vFontStartScreenPos = {};   // 시작 위치
	_float2				m_vFontOffset = {};           // 현재까지 올라온 오프셋
	_float2				m_vFontCurrentScreenPos = {};  // 매 프레임 최종 위치 계산해놓는 변수

#pragma endregion


protected:
	HRESULT				Ready_Components();
	virtual  HRESULT	Ready_PartObjects() = 0;
	virtual  HRESULT	Ready_States()		= 0;

public:
	void	Free()	override;
};

END