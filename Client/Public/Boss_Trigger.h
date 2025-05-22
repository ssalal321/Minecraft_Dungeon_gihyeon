#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
	class CMonster;
	class CBoss_Trigger : public CGameObject
{
public:
	struct BOSS_TRIGGER_DESC : public GAMEOBJECT_DESC
	{
		_float3 triggerPosition = { 0.f, 200.f, 0.f };

		CMonster* pBoss = { nullptr };
	};

private:
	CBoss_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Trigger(const CBoss_Trigger& Prototype);
	~CBoss_Trigger() override = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg = nullptr)	override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

	void		Collided_With(CCollider* pOther, CCollider::COLLISION_STATE eCollisionState) override;

private:
	CShader*		m_pShaderCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };

	_bool			m_bActivated = { true };
	//_bool			m_bBossActivate = { false };
	_float4x4		m_IdentityWorldMatrix = {};

	_float3			m_TriggerPosition = { 0.f, 0.f, 0.f };
	CMonster*		m_pMyBoss = { nullptr };

private:
	HRESULT		Ready_Components();

public:
	static CBoss_Trigger*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	CGameObject*	Clone(void* pArg)	override;
	void	Free()						override;
};

END