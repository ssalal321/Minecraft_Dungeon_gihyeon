#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)
	class CPlayer_Arrow : public CGameObject
{
private:
	CPlayer_Arrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Arrow(const CPlayer_Arrow& Prototype);
	~CPlayer_Arrow() override = default;

public:
	_int		Get_DealPoint() const { return	m_iDealPoint; }

	void		Set_DealPoint(_int iDealPoint) { m_iDealPoint = iDealPoint; }

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg = nullptr)	override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

public:
	void	Shoot(_float4 startPos, _float4 lookPos);
	void	Reset(); // 풀로 돌아갈 때 초기화할 함수

	void	Collided_With(CCollider* pOther, CCollider::COLLISION_STATE eCollisionState) override;

private:
	CShader*	m_pShaderCom = { nullptr };
	CModel*		m_pModelCom = { nullptr };
	CCollider*	m_pColliderCom = { nullptr };

	GAMEOBJECT_DESC*	m_pArrowDesc = { nullptr };;
	_bool				m_bColliderActivating = { false };
	_bool				m_bCollided  = { false };
	_float				m_fResetTimer   = { 0.f };
	_int				m_iDealPoint = { 0 };

	static _int		m_iArrowID;

private:
	HRESULT		Ready_Components();
	HRESULT		Bind_ShaderResources();

public:
	static CPlayer_Arrow*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)	override;
	void	Free()						override;
};

END