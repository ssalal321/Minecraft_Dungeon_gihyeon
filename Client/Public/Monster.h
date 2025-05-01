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
		_uint     iCurrentHP;
		_uint     iMaxHP;
		_int      iDealPoint;
		_float    fAttackableRange;
		_float	  fDetectableRange;
		_bool     bStunned;

		MONSTER_DESC(const _wstring& gameObjectTag, _uint currentHP, const _uint& maxHP, _int dealPoint,
			_float attackableRange, _float detectRange, _bool stunned = false,
			_float rotationPerSec = 0.f, _float speedPerSec = 0.f)
			: GAMEOBJECT_DESC(gameObjectTag, rotationPerSec, speedPerSec), iCurrentHP(currentHP), iMaxHP(maxHP), iDealPoint(dealPoint),
			fAttackableRange(attackableRange), fDetectableRange(detectRange), bStunned(stunned) {
		}

		~MONSTER_DESC() override = default;

		const _uint&	Get_CurrentHP()   const { return iCurrentHP; }
		const _uint&	Get_MaxHP()		  const { return iMaxHP; }
		const _int&		Get_DealPoint()		  const { return iDealPoint; }
		const _float&	Get_AttackRange() const { return fAttackableRange; }
		const _float&	Get_DetectRange() const { return fDetectableRange; }

		void	Modify_CurrentHp(_int iDamageOrHeal)
		{
			if (0 >= iCurrentHP + iDamageOrHeal)
			{
				iCurrentHP = 0;
				return;
			}

			if (iMaxHP <= iCurrentHP + iDamageOrHeal)
			{
				iCurrentHP = iMaxHP;
				return;
			}

			iCurrentHP += iDamageOrHeal;
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

	void	Set_Attacking(_bool bAttacking) { m_bAttacking = bAttacking; }

	void	Set_NextPosition(const _float4& nextPosition)
	{
		m_NextPosition = nextPosition;
	}

	void	Set_Hovered(_bool bPicked) { m_bHovered = bPicked; }

	void		Change_State(const MonsterState& state);
	void		Collided_With(CCollider* pOther, CCollider::COLLISION_STATE eCollisionState)	override;

	_float4		Get_Player_Position(const _wstring& strPlayerGameObjectTag, _uint iPlayerLayerLevelIndex) const;
	_vector		Vec_To_Player(const _wstring& strPlayerGameObjectTag, _uint iPlayerLayerLevelIndex) const;
	_float		Length_To_Player() const;
	_bool		Player_In_DetectRange() const;

protected:
	_uint				m_iState = { static_cast<_uint>(ZOMBIE_STATE::STATE_END) };
	class FSM*			m_pMonsterFSM = { nullptr };
	MONSTER_DESC*		m_pMonsterInfo = { nullptr };
	vector<CState*>     m_StatesVec;

	CNavigation*		m_pNavigationCom = { nullptr };
	//CCollider*			m_pColliderSphereCom = { nullptr };

	_float4				m_NextPosition = { 0.f, 0.f, 0.f, 1.f };
	_bool				m_bAttacking = { false };
	//_bool				m_bHoveringColl = { false };
	_bool				m_bHovered = { false };

protected:
	HRESULT				Ready_Components();
	virtual  HRESULT	Ready_PartObjects() = 0;
	virtual  HRESULT	Ready_States()		= 0;

public:
	void	Free()	override;
};

END