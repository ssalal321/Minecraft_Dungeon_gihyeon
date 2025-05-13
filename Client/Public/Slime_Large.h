#pragma once
#include "Client_Defines.h"
#include "Monster.h"

/* 플레이어라는 객체를 구성하기위한 파츠들을 모아서 쥐고 있는 객체. */

BEGIN(Client)
class CState;

class CSlime_Large final : public CMonster
{
public:
	/*struct SLIME_LARGE_DESC : public GAMEOBJECT_DESC
	{
		_int     uiCurrentHP;
		_int     uiMaxHP;
		_int     iArrowDealPoint;
		_float   fAttackableRange;
		_bool    bStunned;

		SLIME_LARGE_DESC(const _tchar* GameObjectTag, _int currentHP, _int maxHP, _int attackPoint,
			_float effectiveRange, _bool stunned = false,
			_float rotationPerSec = 0.f, _float speedPerSec = 0.f)
			: GAMEOBJECT_DESC(GameObjectTag, rotationPerSec, speedPerSec), uiCurrentHP(currentHP), uiMaxHP(maxHP), iArrowDealPoint(attackPoint),
			fAttackableRange(effectiveRange), bStunned(stunned) {
		}

		~SLIME_LARGE_DESC() override = default;
	};*/

private:
	CSlime_Large(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSlime_Large(const CSlime_Large& Prototype);
	~CSlime_Large() override = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;
	
private:
	_uint			m_iState = { static_cast<_uint>(SLIME_LARGE_STATE::STATE_END) };

	static _int		m_iSlime_LargeID;

private:
	HRESULT		Ready_PartObjects()  override;
	HRESULT		Ready_States()		 override;

public:
	static CSlime_Large* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)	override;
	void	Free()						override;
};

END