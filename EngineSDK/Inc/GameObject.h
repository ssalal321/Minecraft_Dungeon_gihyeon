#pragma once

#include "Transform.h"
#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagGameObjectDesc : public CTransform::TRANSFORM_DESC
	{
		_wstring	strGameObjectTag = {};

		tagGameObjectDesc(const _wstring& gameObjectTag = TEXT(""), _float rotationPerSec = 0.f, _float speedPerSec = 0.f)
			: tagTransformDesc(rotationPerSec, speedPerSec), strGameObjectTag(gameObjectTag) {}

		/*tagGameObjectDesc(const tagGameObjectDesc &other)
			: CTransform::TRANSFORM_DESC(other),
			  pGameObjectTag(other.pGameObjectTag) {}*/

		~tagGameObjectDesc() override = default;

	}GAMEOBJECT_DESC;

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() override = default;

public:
	const _wstring&		Get_GameObjectTag() { return m_strGameObjectTag; }

public:
	virtual   HRESULT	Initialize_Prototype();
	virtual   HRESULT	Initialize(void* pArg);
	virtual   void		Priority_Update(_float fTimeDelta);
	virtual   void		Update(_float fTimeDelta);
	virtual   void		Late_Update(_float fTimeDelta);
	virtual   HRESULT	Render();

public:
	class CComponent*	Find_Component(const _wstring& strComponentTag);
	void				Erase_Component(const wstring& strComponentTag);
	virtual  void		Collided_With(CCollider* pOther, CCollider::COLLISION_STATE eCollisionState);

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };

protected:
	_wstring	m_strGameObjectTag = {};
	map<const _wstring, class CComponent*>	m_Components;

	_bool		m_bActive = { false };

protected:
	CComponent*		Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag,
								  const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);

public:
	virtual  CGameObject*	Clone(void* pArg) = 0;
	virtual  void			Free()	override;



};

END