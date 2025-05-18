#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
	class CLevel_Trigger : public CGameObject
{
public:
	struct LEVEL_TRIGGER_DESC : public GAMEOBJECT_DESC
	{
		_float3 triggerPosition = { 0.f, 200.f, 0.f };
	};

private:
	CLevel_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLevel_Trigger(const CLevel_Trigger& Prototype);
	~CLevel_Trigger() override = default;

public:
	_bool		Get_Level_Change() const { return m_bSceneChanged; }

	void		Set_Level_Change(_bool bSceneChanged) { m_bSceneChanged = bSceneChanged; }

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
	_bool			m_bSceneChanged = { false };
	_float4x4		m_IdentityWorldMatrix = {};

	_float3			m_TriggerPosition = { 0.f, 0.f, 0.f };

private:
	HRESULT		Ready_Components();

public:
	static CLevel_Trigger*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	CGameObject*	Clone(void* pArg)	override;
	void	Free()						override;
};

END