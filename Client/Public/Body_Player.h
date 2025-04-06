#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CBody_Player final : public CPartObject
{
public:
	typedef struct tagBodyPlayerDesc : public CPartObject::PARTOBJECT_DESC
	{
		const _uint* pState = { nullptr };
	}BODY_PLAYER_DESC;
private:
	CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Player(const CBody_Player& Prototype);
	~CBody_Player() override = default;

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

	_uint				m_iPassIndex = {};

private:
	const _uint*		m_pTargetState = { nullptr };


private:
	HRESULT		Ready_Components();
	HRESULT		Bind_ShaderResources();

public:
	static	CBody_Player*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)	override;
	void			Free()				override;
};

END