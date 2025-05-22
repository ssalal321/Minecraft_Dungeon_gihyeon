#pragma once
#include "Client_Defines.h"
#include "Monster.h"

/* 플레이어라는 객체를 구성하기위한 파츠들을 모아서 쥐고 있는 객체. */

BEGIN(Client)
class CState;

class CSlime_Large final : public CMonster
{
public:
	struct SLIME_LARGE_DESC
	{
		_float4  slimeLargePosition = { 0.f, 0.f, 0.f, 1.f };
		_int	 currentCellIndex = { 800 };
	};

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