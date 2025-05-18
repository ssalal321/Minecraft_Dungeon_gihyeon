#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

namespace Client
{
	class CBulletPool_Monster;
}

BEGIN(Engine)
	class CModel;
END

BEGIN(Client)
	class CSlime_Cauldron_Bullet : public CGameObject
{
private:
	CSlime_Cauldron_Bullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSlime_Cauldron_Bullet(const CSlime_Cauldron_Bullet& Prototype);
	~CSlime_Cauldron_Bullet() override = default;

public:
	_int		Get_DealPoint() const { return m_iDealPoint; }

	void		Set_Pool(CBulletPool_Monster* pPool) { m_pBulletPool = pPool; }
	void		Set_DealPoint(_int iDealPoint) { m_iDealPoint = iDealPoint; }

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg = nullptr)	override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

public:
	void	Shoot(_float3 startPos, _float3 endPos);
	void	Return_To_Pool(); // 풀로 돌아갈 때 초기화할 함수

	void	Collided_With(CCollider* pOther, CCollider::COLLISION_STATE eCollisionState) override;

private:
	CShader*		m_pShaderCom = { nullptr };
	CModel*			m_pModelCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };

	CBulletPool_Monster* m_pBulletPool = { nullptr };

	GAMEOBJECT_DESC*	m_pBulletDesc = { nullptr };;
	_bool				m_bAttacking = { false };
	_bool				m_bCollided = { false };
	_bool				m_bActive	= { false };
	_float				m_fResetTimer = 0.f;
	_int				m_iDealPoint = { 0 };

	_float3				m_EndPos = {};

	static _int		m_iBulletID;

private:
	HRESULT		Ready_Components();
	HRESULT		Bind_ShaderResources();

public:
	static CSlime_Cauldron_Bullet*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)	override;
	void	Free()						override;
};

END