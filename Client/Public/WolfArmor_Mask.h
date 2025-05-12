#pragma once
#include "Client_Defines.h"
#include "PartObject.h"


BEGIN(Engine)
class CModel;
END

BEGIN(Client)
	class CWolfArmor_Mask final : public CPartObject
{
public:
	typedef struct tagWolfArmorMaskDesc : PARTOBJECT_DESC
	{
		_bool	bPartActive = { false };

		const _float4x4* pMaskSocketMatrix = { nullptr };

	}WOLFARMOR_MASK_DESC;

private:
	CWolfArmor_Mask(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWolfArmor_Mask(const CWolfArmor_Mask& Prototype);
	~CWolfArmor_Mask() override = default;

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
	static  CWolfArmor_Mask*	 Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)	override;
	void	Free()						override;
};

END