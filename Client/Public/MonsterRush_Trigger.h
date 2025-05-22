#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
	class CMonster;
	class CMonsterRush_Trigger : public CGameObject
{
public:
	struct MONSTERRUSH_TRIGGER_DESC : public GAMEOBJECT_DESC
	{
		_float3 triggerPosition = { 0.f, 200.f, 0.f };
	};

private:
	CMonsterRush_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterRush_Trigger(const CMonsterRush_Trigger& Prototype);
	~CMonsterRush_Trigger() override = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg = nullptr)	override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

	void		Collided_With(CCollider* pOther, CCollider::COLLISION_STATE eCollisionState) override;

public:
	void		Add_Monster(CMonster* pMonster);
	void		Notify_Monster_Died(CMonster* pMonster);

private:
	CShader*		m_pShaderCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };

	vector<CMonster*>	m_MonstersToActivate;

	_bool			m_bActivated = { true };
	
	_float4x4		m_IdentityWorldMatrix = {};

	_float3			m_TriggerPosition = { 0.f, 0.f, 0.f };
	
private:
	HRESULT		Ready_Components();

public:
	static CMonsterRush_Trigger*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	CGameObject*	Clone(void* pArg)	override;
	void	Free()						override;
};

END