#pragma once
#include "Client_Defines.h"
#include "Monster.h"

/* 플레이어라는 객체를 구성하기위한 파츠들을 모아서 쥐고 있는 객체. */

BEGIN(Client)
	class CArrowPool_Monster;
	class CState;

class CSkeleton final : public CMonster
{
private:
	CSkeleton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkeleton(const CSkeleton& Prototype);
	~CSkeleton() override = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;
	
private:
	_uint		m_iState = { static_cast<_uint>(ZOMBIE_STATE::STATE_END) };

	CArrowPool_Monster*		m_pArrowPool_Monster = { nullptr };

	static _int		m_iSkeletonID;

private:
	HRESULT		Ready_PartObjects()  override;
	HRESULT		Ready_States()		 override;

public:
	static	CSkeleton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)	override;
	void	Free()						override;
};

END