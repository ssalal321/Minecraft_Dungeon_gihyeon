#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
	class CBoss_Trigger : public CGameObject
{
public:
	struct BOSS_TRIGGER_DESC : public GAMEOBJECT_DESC
	{
		_float3 triggerPosition = { 0.f, 200.f, 0.f };
	};

private:
	CBoss_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Trigger(const CBoss_Trigger& Prototype);
	~CBoss_Trigger() override = default;

public:
	_bool*		Get_Boss_Activated() { return &m_bBossActivate; }

	void		Set_Boss_Activate(_bool bSceneChanged) { m_bBossActivate = bSceneChanged; }

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
	_bool			m_bBossActivate = { false };
	_float4x4		m_IdentityWorldMatrix = {};

	_float3			m_TriggerPosition = { 0.f, 0.f, 0.f };

private:
	HRESULT		Ready_Components();

public:
	static CBoss_Trigger*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	CGameObject*	Clone(void* pArg)	override;
	void	Free()						override;
};

END