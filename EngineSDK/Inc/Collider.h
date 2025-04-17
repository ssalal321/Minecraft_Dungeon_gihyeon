#pragma once

#include "Component.h"

BEGIN(Engine)
class CGameObject;

class ENGINE_DLL CCollider final : public CComponent
{
private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& Prototype);
	~CCollider() override = default;

public:
	void	Set_IsCollision(_bool bIsCollision) { m_isCollision = bIsCollision; }
	void	Set_OwnerTag(const _wstring& strOwnerTag) { m_strOwnerTag = strOwnerTag; }

	const _wstring&		Get_OwnerTag() { return m_strOwnerTag; }

public:
	virtual HRESULT Initialize_Prototype(COLLIDER eColliderType);
	HRESULT		Initialize(void* pArg) override;
	void		Update();

#ifdef _DEBUG
	HRESULT Render();
#endif

public:
	_bool	Intersect(CCollider* pTargetCollider);
	void	Is_Hit(CCollider* pOther);

private:
	COLLIDER			m_eColliderType = { COLLIDER::TYPE_END };
	class CBounding*	m_pBounding		= { nullptr };
	_bool				m_isCollision	= { false };
	_wstring			m_strOwnerTag	= {};

	CGameObject*		m_pOwnerGameObject	= { nullptr };
	_float4x4*			m_CombinedWorldMatrix = { nullptr };

#ifdef _DEBUG
private:
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
