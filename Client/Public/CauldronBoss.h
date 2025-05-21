#pragma once
#include "Client_Defines.h"
#include "Monster.h"

/* 플레이어라는 객체를 구성하기위한 파츠들을 모아서 쥐고 있는 객체. */

namespace Engine
{
	class CUIObject;
}

namespace Client
{
	class CCauldronBossHP;
}

namespace Client
{
	class CBody_CauldronBoss;
}

BEGIN(Client)
	class CBoss_Trigger;
class CState;

class CCauldronBoss final : public CMonster
{
public:
	struct CAULDRONBOSS_DESC
	{
		_float4  slimeCauldronPosition = { 0.f, 0.f, 0.f, 1.f };
	};

private:
	CCauldronBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCauldronBoss(const CCauldronBoss& Prototype);
	~CCauldronBoss() override = default;

public:
	CUIObject*	Get_My_HPUIObject() { return m_pCauldronBossHP; }

	void		Set_My_HPUIObject(CUIObject* pHPUIObject) { m_pCauldronBossHP = pHPUIObject; }

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;
	
private:
	_uint			m_iState = { static_cast<_uint>(CAULDRONBOSS_STATE::STATE_END) };

	static _int		m_iCauldronBossID;

	CUIObject*		m_pCauldronBossHP = { nullptr };

private:
	HRESULT		Ready_PartObjects()  override;
	HRESULT		Ready_States()		 override;

public:
	static CCauldronBoss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)	override;
	void	Free()						override;
};

END