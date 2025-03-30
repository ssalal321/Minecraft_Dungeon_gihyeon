#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Cube;
END

BEGIN(Client)

class CSky final : public CGameObject
{
private:
	CSky(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSky(const CSky& Prototype);
	~CSky() override = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

private:	
	
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CVIBuffer_Cube*		m_pVIBufferCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSky*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)	override;
	void	Free()						override;
};

END