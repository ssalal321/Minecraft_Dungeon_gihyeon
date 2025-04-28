#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CContainerObject;

class ENGINE_DLL CPartObject abstract : public CGameObject
{
public:
	typedef struct tagPartObject : public CGameObject::GAMEOBJECT_DESC
	{
		CContainerObject*	pContainerObject = {nullptr};
		const _float4x4*	pParentWorldMatrix = {nullptr};

		_bool*		pContainerObjAttacking = { nullptr };

	}PARTOBJECT_DESC;

protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartObject(const CPartObject& Prototype);
	~CPartObject() override = default;

public:
	CContainerObject* Get_ContainerObject() { return m_pContainerObject; }

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

protected:
	class CContainerObject*		m_pContainerObject = { nullptr };

	const _float4x4*	m_pParentWorldMatrix = { nullptr };
	_float4x4			m_CombinedWorldMatrix = {};
	_bool*		m_pContainerObjAttacking = { nullptr };

public:
	CGameObject* Clone(void* pArg)	override = 0;
	void	Free()					override;
};

END