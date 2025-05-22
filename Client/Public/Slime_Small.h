#pragma once
#include "Client_Defines.h"
#include "Monster.h"

/* 플레이어라는 객체를 구성하기위한 파츠들을 모아서 쥐고 있는 객체. */

BEGIN(Client)
class CState;

class CSlime_Small final : public CMonster
{
public:
	struct SLIME_SMALL_DESC
	{
		_float4  slimeSmallPosition = { 0.f, 0.f, 0.f, 1.f };
		_int	 currentCellIndex = {};
	};

private:
	CSlime_Small(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSlime_Small(const CSlime_Small& Prototype);
	~CSlime_Small() override = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;
	
private:
	_uint			m_iState = { static_cast<_uint>(SLIME_SMALL_STATE::STATE_END) };

	static _int		m_iSlime_SmallID;

private:
	HRESULT		Ready_PartObjects()  override;
	HRESULT		Ready_States()		 override;

public:
	static CSlime_Small* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)	override;
	void	Free()						override;
};

END