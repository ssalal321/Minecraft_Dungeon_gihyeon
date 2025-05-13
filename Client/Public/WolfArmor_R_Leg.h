#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)
	class CWolfArmor_R_Leg final : public CPartObject
{
public:
	typedef struct tagWolfArmorRLegDesc : PARTOBJECT_DESC
	{
		_bool	bPartActive = { false };

		const _float4x4* pRLegSocketMatrix = { nullptr };

	}WOLFARMOR_R_LEG_DESC;

private:
	CWolfArmor_R_Leg(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWolfArmor_R_Leg(const CWolfArmor_R_Leg& Prototype);
	~CWolfArmor_R_Leg() override = default;

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
	static  CWolfArmor_R_Leg*	 Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)	override;
	void	Free()						override;
};

END