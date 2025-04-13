#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
	class CVIBuffer_Cube;
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
	CShader*		m_pShader_MeshCom	 = { nullptr };
	CModel*			m_pModelCom		 = { nullptr };
	CNavigation*	m_pNavigationCom = { nullptr };

	_int			m_iPointNum		 = {};
	_float3			m_fCellPoints[3] = {};

	static  _bool	m_bLineRender;
	static	_uint	m_iShaderPass;  // ¼±¾ð
	static	_bool	m_bClickActive;;

private:
	HRESULT		Ready_Components();
	HRESULT		Bind_ShaderResources();

public:
	static CLoungeMap*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*		Clone(void* pArg)	override;
	void	Free()							override;
};

END