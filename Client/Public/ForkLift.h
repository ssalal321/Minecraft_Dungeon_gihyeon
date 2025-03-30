#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CForkLift final : public CGameObject
{
private:
	CForkLift(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CForkLift(const CForkLift& Prototype);
	virtual ~CForkLift() = default;

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
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CForkLift* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END