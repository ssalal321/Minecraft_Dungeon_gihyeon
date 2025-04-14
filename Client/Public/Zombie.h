#pragma once
#include "Client_Defines.h"
#include "Monster.h"

/* 플레이어라는 객체를 구성하기위한 파츠들을 모아서 쥐고 있는 객체. */

BEGIN(Engine)
class	CCollider;
class	CNavigation;
END


BEGIN(Client)
class CState;

class CZombie final : public CMonster
{
public:
	struct ZOMBIE_DESC : public GAMEOBJECT_DESC
	{
		_int     iCurrentHP;
		_int     iMaxHP;
		_int     iAttackPoint;
		_float   fEffectiveRange;
		_bool    bStunned;

		ZOMBIE_DESC(const _tchar* GameObjectTag, _int currentHP, _int maxHP, _int attackPoint,
			_float effectiveRange, _bool stunned = false,
			_float rotationPerSec = 0.f, _float speedPerSec = 0.f)
			: GAMEOBJECT_DESC(GameObjectTag, rotationPerSec, speedPerSec), iCurrentHP(currentHP), iMaxHP(maxHP), iAttackPoint(attackPoint),
			fEffectiveRange(effectiveRange), bStunned(stunned) {
		}

		~ZOMBIE_DESC() override = default;
	};

private:
	CZombie(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CZombie(const CZombie& Prototype);
	~CZombie() override = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;
	
private:
	_uint				m_iState = { static_cast<_uint>(ZOMBIE_STATE::STATE_END) };
	
	ZOMBIE_DESC*		m_pZombieInfo = { nullptr };

	_float4				m_NextPosition = {0.f, 0.f, 0.f, 1.f};

private:
	//HRESULT	Ready_Components();
	HRESULT		Ready_PartObjects();
	HRESULT		Ready_States();

public:
	static CZombie* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)	override;
	void	Free()						override;
};

END