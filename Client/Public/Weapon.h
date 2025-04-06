#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CWeapon final : public CPartObject
{
public:
	typedef struct tagWeaponDesc : public CPartObject::PARTOBJECT_DESC
	{
		const _uint* pState = { nullptr };
		const _float4x4* pSocketMatrix = { nullptr };
	}WEAPON_DESC;
private:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& Prototype);
	virtual ~CWeapon() = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

private:	
	
	CShader*			m_pShaderCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };

private:
	const _float4x4*	m_pSocketMatrix = { nullptr };
	const _uint* m_pTargetState = { nullptr };
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END