#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)
	class CWolfArmor_L_Leg final : public CPartObject
{
public:
	typedef struct tagWolfArmorLLegDesc : PARTOBJECT_DESC
	{
		_bool	bPartActive = { false };

		const _float4x4* pLLegSocketMatrix = { nullptr };

	}WOLFARMOR_L_LEG_DESC;

private:
	CWolfArmor_L_Leg(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWolfArmor_L_Leg(const CWolfArmor_L_Leg& Prototype);
	~CWolfArmor_L_Leg() override = default;

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
	static  CWolfArmor_L_Leg*	 Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)	override;
	void	Free()						override;
};

END