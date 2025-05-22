#pragma once
#include "Client_Defines.h"
#include "Monster.h"

/* 플레이어라는 객체를 구성하기위한 파츠들을 모아서 쥐고 있는 객체. */

namespace Client
{
	class CBulletPool_Monster;
}

BEGIN(Client)
	class CState;

class CSlime_Cauldron final : public CMonster
{
public:
	struct SLIME_CAULDRON_DESC
	{
		_float4  slimeCauldronPosition = { 0.f, 0.f, 0.f, 1.f };
	};

private:
	CSlime_Cauldron(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSlime_Cauldron(const CSlime_Cauldron& Prototype);
	~CSlime_Cauldron() override = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

private:
	CBulletPool_Monster*	m_pBulletPool_Monster = { nullptr };

	_uint			m_iState = { static_cast<_uint>(SLIME_CAULDRON_STATE::STATE_END) };

	static _int		m_iSlimeCauldronID;

	_float		m_fLifeTime = {};

private:
	HRESULT		Ready_PartObjects()  override;
	HRESULT		Ready_States()		 override;

public:
	static CSlime_Cauldron* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)	override;
	void	Free()						override;
};

END