#pragma once

#include <unordered_set>

#include "Component.h"

BEGIN(Engine)
	class CBounding;
	class CGameObject;

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum COLLISION_STATE { ENTER, STAY, EXIT, STATE_END };

private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& Prototype);
	~CCollider() override = default;

public:
	void	Set_IsCollision(_bool bIsCollision) { m_bIsCollision = bIsCollision; }
	void	Set_OwnerTag(const _wstring& strOwnerTag) { m_strOwnerTag = strOwnerTag; }
	void	Set_Collider_Off(_bool ColliderOff) { m_bColliderOff = ColliderOff; }

	const _wstring&	Get_OwnerTag() { return m_strOwnerTag; }
	CBounding*	Get_Bounding() const { return m_pBounding; }
	_bool		Get_Collider_Off() const { return m_bColliderOff; }
	_bool		Get_OtherAttacking() const { return *m_bContainerObjAttacking; }

public:
	virtual HRESULT Initialize_Prototype(COLLIDER eColliderType);
	HRESULT		Initialize(void* pArg) override;
	void		Update();

#ifdef _DEBUG
	HRESULT Render();
#endif

public:
	_bool	Intersect(CCollider* pTargetCollider);
	void	Collided_With(CCollider* pOther);
	void	Process_Collisions();

private:
	COLLIDER			m_eColliderType = { COLLIDER::TYPE_END };
	class CBounding*	m_pBounding		= { nullptr };
	_bool				m_bIsCollision	= { false };
	_wstring			m_strOwnerTag	= {};
	_bool				m_bColliderOff	= { false };

	unordered_set<CCollider*>	m_prevCollisions;
	unordered_set<CCollider*>	m_currCollisions;

	CGameObject*		m_pOwnerGameObject	= { nullptr };
	_float4x4*			m_CombinedWorldMatrix = { nullptr };
	_bool*				m_bContainerObjAttacking = { nullptr };

#ifdef _DEBUG
	PrimitiveBatch<VertexPositionColor>*	m_pBatch = { nullptr };
	BasicEffect*			m_pEffect		= { nullptr };
	ID3D11InputLayout*		m_pInputLayout  = { nullptr };
#endif

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER eColliderType);
	CComponent* Clone(void* pArg)	override;
	void	Free()					override;

};


END
