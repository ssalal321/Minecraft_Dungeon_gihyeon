#pragma once
#include "ChestIcon.h"
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)
class CChestIcon;

class CLobbyChest : public CGameObject
{
public:
	struct LOBBY_CHEST_DESC : public GAMEOBJECT_DESC
	{
		_float4 worldPosition = { 0.f, 0.f, 0.f, 1.f};

		vector<CChestIcon::CHEST_ICON_DESC>	 pChestIconDescs;

		_uint	uiMoneyNum = {};
	};

private:
	CLobbyChest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLobbyChest(const CLobbyChest& Prototype);
	~CLobbyChest() override = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg = nullptr)	override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

	void		Collided_With(CCollider* pOther, CCollider::COLLISION_STATE eCollisionState) override;

private:
	CShader*		m_pShaderCom = { nullptr };
	CModel*			m_pClosedModelCom = { nullptr };
	CModel*			m_pOpenedModelCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };

	_bool			m_bActivated = { true };
	_bool			m_bOpened = { false };
	_float4x4		m_IdentityWorldMatrix = {};

	_float4			m_WorldPosition = { 0.f, 0.f, 0.f, 1.f };

	vector<CChestIcon::CHEST_ICON_DESC>		m_pChestIconDescs;
	vector<CChestIcon*>		m_pChestIcons;
	_uint			m_uiMoneyNum = {};

private:
	HRESULT		Ready_Components();
	HRESULT		Bind_ShaderResources();

	void		Pop_Out_Items();

public:
	static CLobbyChest*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)	override;
	void	Free()						override;
};

END