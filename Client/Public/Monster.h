#pragma once
#include "Client_Defines.h"
#include "ContainerObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
	class CState;

class CMonster abstract : public CContainerObject
{
public:
	struct MONSTER_DESC : public GAMEOBJECT_DESC
	{
		_int     iCurrentHP;
		_int     iMaxHP;
		_int     iAttackPoint;
		_float   fAttackableRange;
		_float	 fDetectableRange;
		_bool    bStunned;

		MONSTER_DESC(const _tchar* GameObjectTag, _int currentHP, _int maxHP, _int attackPoint,
			_float attackableRange, _float detectRange, _bool stunned = false,
			_float rotationPerSec = 0.f, _float speedPerSec = 0.f)
			: GAMEOBJECT_DESC(GameObjectTag, rotationPerSec, speedPerSec), iCurrentHP(currentHP), iMaxHP(maxHP), iAttackPoint(attackPoint),
			fAttackableRange(attackableRange), fDetectableRange(detectRange), bStunned(stunned) {
		}

		~MONSTER_DESC() override = default;

		const _float&	Get_AttackRange() const { return fAttackableRange; }
		const _float&	Get_DetectRange() const { return fDetectableRange; }
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
	vector<CState*>   Get_StateVec() { return m_StatesVec; }

	const _float4& Get_NextPosition() const { return m_NextPosition; }

	void	Set_NextPosition(const _float4& nextPosition)
	{
		m_NextPosition = nextPosition;
	}

	void		Change_State(ZOMBIE_STATE monsterState);

	_float4		Get_Player_Position(const _wstring& strPlayerPrototypeTag, _uint iPlayerLayerLevelIndex) const;
	_vector		Vec_To_Player(const _wstring& strPlayerPrototypeTag, _uint iPlayerLayerLevelIndex) const;
	_float		Length_To_Player(const _wstring& strPlayerPrototypeTag, _uint iPlayerLayerLevelIndex) const;
	_bool		Player_In_DetectRange(const _wstring& strPrototypeTag, _uint iLayerLevelIndex) const;

protected:
	_uint				m_iState = { static_cast<_uint>(ZOMBIE_STATE::STATE_END) };
	class FSM*			m_pMonsterFSM = { nullptr };
	MONSTER_DESC*		m_pMonsterInfo = { nullptr };
	vector<CState*>     m_StatesVec;

	CNavigation*		m_pNavigationCom = { nullptr };
	_float4				m_NextPosition = { 0.f, 0.f, 0.f, 1.f };

protected:
	HRESULT				Ready_Components();
	virtual  HRESULT	Ready_PartObjects() = 0;
	virtual  HRESULT	Ready_States()		= 0;

public:
	void	Free()	override;
};

END