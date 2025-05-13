#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)
	class CWolfArmor_Body final : public CPartObject
{
public:
	typedef struct tagWolfArmorBodyDesc : PARTOBJECT_DESC
	{
		_bool	bPartActive = { false };

		const _float4x4* pBodySocketMatrix = { nullptr };

	}WOLFARMOR_BODY_DESC;

private:
	CWolfArmor_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWolfArmor_Body(const CWolfArmor_Body& Prototype);
	~CWolfArmor_Body() override = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

private:
	CShader*	m_pShaderCom = { nullptr };
	CModel*		m_pModelCom = { nullptr };

	const _float4x4*	m_pSocketMatrix = { nullptr };

	_bool		m_bPartActive = { false };

private:
	HRESULT		Ready_Components();
	HRESULT		Bind_ShaderResources();

public:
	static  CWolfArmor_Body*	 Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)	override;
	void	Free()						override;
};

END