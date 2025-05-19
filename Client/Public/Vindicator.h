#pragma once
#include "Client_Defines.h"
#include "Monster.h"

/* 플레이어라는 객체를 구성하기위한 파츠들을 모아서 쥐고 있는 객체. */

BEGIN(Client)
class CState;

class CVindicator final : public CMonster
{
public:
	struct VINDICATOR_DESC
	{
		_float4  babyZombiePosition = { 0.f, 0.f, 0.f, 1.f };
	};

private:
	CVindicator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVindicator(const CVindicator& Prototype);
	~CVindicator() override = default;

public:
	_bool   Get_Retreating() const { return m_bRetreating; }

	void    Set_Retreating(_bool bRetreating) { m_bRetreating = bRetreating; }

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;
	
private:
	_uint			m_iState = { static_cast<_uint>(VINDICATOR_STATE::STATE_END) };

	static _int		m_iVindicatorID;

	_bool			m_bRetreating = { false };

private:
	HRESULT		Ready_PartObjects()  override;
	HRESULT		Ready_States()		 override;

public:
	static CVindicator* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)	override;
	void	Free()						override;
};

END