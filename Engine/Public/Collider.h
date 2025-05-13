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
	void	Set_ColliderTag(const _wstring& strColliderTag) { m_strColliderTag = strColliderTag; }
	void	Set_ColliderActive(_bool ColliderActive) { m_bColliderActive = ColliderActive; }
	void	Set_MouseCollider(_bool ColliderForMouse) { m_bMouseCollider = ColliderForMouse; }
	void	Set_AllowSameGroupCollision(_bool bAllowCollision) { m_bAllowInGroupCollision = bAllowCollision; }

	const _wstring&		Get_ColliderTag() { return m_strColliderTag; }
	COLLIDER			Get_ColliderType() const { return m_eColliderType; }
	CBounding*			Get_Bounding() const { return m_pBounding; }
	_bool				Get_ColliderActive() const { return m_bColliderActive; }
	_bool				Get_MouseCollider() const { return m_bMouseCollider; }
	_bool				Get_Other_Collision_Activated() const { return *m_bCollisionActivated; }
	CGameObject*		Get_OwnerObject() const { return m_pOwnerGameObject; }
	_bool				Get_AllowSameGroupCollision() const { return m_bAllowInGroupCollision; }

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
	void	Clear_Collision_State();

private:
	COLLIDER			m_eColliderType = { COLLIDER::TYPE_END };
	class CBounding*	m_pBounding		= { nullptr };
	_bool				m_bIsCollision	= { false };
	_wstring			m_strColliderTag	= {};
	_bool				m_bColliderActive	= { true };
	_bool				m_bMouseCollider	= { false };
	_wstring			m_bTotalGroup		= {};

	unordered_set<CCollider*>	m_prevCollisions;
	unordered_set<CCollider*>	m_currCollisions;

	CGameObject*		m_pOwnerGameObject	= { nullptr };
	_float4x4*			m_CombinedWorldMatrix = { nullptr };
	_bool*				m_bCollisionActivated = { nullptr };

	_bool				m_bAllowInGroupCollision = { false };

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
