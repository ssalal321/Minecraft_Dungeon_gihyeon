#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CLoungeMap final : public CGameObject
{
private:
	CLoungeMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLoungeMap(const CLoungeMap& Prototype);
	~CLoungeMap() override = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

private:	
	CShader*		m_pShaderCom	 = { nullptr };
	CModel*			m_pModelCom		 = { nullptr };
	CNavigation*	m_pNavigationCom = { nullptr };

private:
	HRESULT		Ready_Components();
	HRESULT		Bind_ShaderResources();

public:
	static CLoungeMap*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*		Clone(void* pArg)	override;
	void	Free()							override;
};

END