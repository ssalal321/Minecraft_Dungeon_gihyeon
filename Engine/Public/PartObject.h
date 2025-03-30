#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CPartObject abstract : public CGameObject
{
public:
	typedef struct tagPartObject : public CGameObject::GAMEOBJECT_DESC
	{
		const _float4x4* pParentWorldMatrix = {nullptr};
	}PARTOBJECT_DESC;

protected:
	CPartObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CPartObject(const CPartObject& Prototype);
	~CPartObject() override = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

protected:	
	const _float4x4*	m_pParentWorldMatrix	= { nullptr };
	_float4x4			m_CombinedWorldMatrix	= {};

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END