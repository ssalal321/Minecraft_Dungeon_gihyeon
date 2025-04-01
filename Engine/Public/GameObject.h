#pragma once

#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagGameObjectDesc : public CTransform::TRANSFORM_DESC
	{
		const _tchar* pGameObjectTag;

		tagGameObjectDesc(const _tchar* gameObjectTag = TEXT(""), _float rotationPerSec = 0.f, _float speedPerSec = 0.f)
			: tagTransformDesc(rotationPerSec, speedPerSec), pGameObjectTag(gameObjectTag) {}

		tagGameObjectDesc(const tagGameObjectDesc &other)
			: CTransform::TRANSFORM_DESC(other),
			  pGameObjectTag(other.pGameObjectTag) {}

		~tagGameObjectDesc() override = default;

	}GAMEOBJECT_DESC;

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() override = default;

public:
	virtual   HRESULT	Initialize_Prototype();
	virtual   HRESULT	Initialize(void* pArg);
	virtual   void		Priority_Update(_float fTimeDelta);
	virtual   void		Update(_float fTimeDelta);
	virtual   void		Late_Update(_float fTimeDelta);
	virtual   HRESULT	Render();

public:
	class CComponent*	Find_Component(const _wstring& strComponentTag);

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };

protected:
	_tchar									m_szGameObjectTag[MAX_PATH] = {};
	map<const _wstring, class CComponent*>	m_Components;

protected:
	HRESULT		Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag,
							  const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);


public:
	virtual  CGameObject*	Clone(void* pArg) = 0;
	virtual  void			Free()	override;



};

END