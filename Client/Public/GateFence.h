#pragma once
#include "ChestIcon.h"
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)
class CChestIcon;

class CGateFence : public CGameObject
{
public:
	struct GATEFENCE_DESC : public GAMEOBJECT_DESC
	{
		_float4 worldPosition = { 0.f, 0.f, 0.f, 1.f};
	};

private:
	CGateFence(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGateFence(const CGateFence& Prototype);
	~CGateFence() override = default;

public:
	_bool		Get_Appearing() const { return m_bAppearing; }
	_bool		Get_Dying() const { return m_bDying; }

	void		Set_Appearing(_bool bAppearing) { m_bAppearing = bAppearing; }
	void		Set_Dying(_bool bDying) { m_bDying = bDying; }

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg = nullptr)	override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

private:
	CShader*		m_pShaderCom = { nullptr };
	CModel*			m_pModelCom = { nullptr };

	_bool			m_bActivated = { true };
	_bool			m_bOpened = { false };

	_float4			m_WorldPosition = { 0.f, 0.f, 0.f, 1.f };

	_bool			m_bAppearing = { false };
	_bool			m_bDying = { false };

#pragma region SHADER
	_float			m_fAppearTime = {};
	_float			m_fAppearDuration = {};  // 몇 초 걸릴지

	_float			m_fDeathTime = {};
	_float			m_fDeathDuration = {};
#pragma endregion

private:
	HRESULT		Ready_Components();
	HRESULT		Bind_ShaderResources(_uint iPassIndex);

public:
	static CGateFence*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)	override;
	void	Free()						override;
};

END