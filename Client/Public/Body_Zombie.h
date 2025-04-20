#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
	class CCollider;
	class CShader;
	class CModel;
END

BEGIN(Client)

class CBody_Zombie final : public CPartObject
{
public:
	typedef struct tagBodyZombieDesc : public CPartObject::PARTOBJECT_DESC
	{
		const _uint* pState = { nullptr };
	}BODY_ZOMBIE_DESC;

private:
	CBody_Zombie(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Zombie(const CBody_Zombie& Prototype);
	~CBody_Zombie() override = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

public:
	void		Collided_With(CCollider* pOther, CCollider::COLLISION_STATE eCollisionState) override;

private:	
	CShader*			m_pShaderCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };

	CCollider*			m_pColliderOBBCom = { nullptr };

	_uint				m_iPassIndex = {};

private:
	const _uint*		m_pTargetState = { nullptr };


private:
	HRESULT		Ready_Components();
	HRESULT		Bind_ShaderResources();

public:
	static	CBody_Zombie*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)	override;
	void			Free()				override;
};

END