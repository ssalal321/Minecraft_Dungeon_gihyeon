#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CMonster final : public CGameObject
{
private:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& Prototype);
	virtual ~CMonster() = default;

public:
	virtual  HRESULT	Initialize_Prototype()				override;
	virtual  HRESULT	Initialize(void* pArg)				override;
	virtual  void		Priority_Update(_float fTimeDelta)	override;
	virtual  void		Update(_float fTimeDelta)			override;
	virtual  void		Late_Update(_float fTimeDelta)		override;
	virtual  HRESULT	Render()							override;

private:	
	
	CShader*	m_pShaderCom	= { nullptr };
	CModel*		m_pModelCom		= { nullptr };

	_uint		m_iPassIndex	= {};


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END