#pragma once
#include "Client_Defines.h"
#include "Item.h"

BEGIN(Client)

class CWeapon_ShortBow final : public CItem
{
public:
	typedef struct tagWeaponShortBowDesc : ITEM_DESC
	{


	}WEAPON_SHORTBOW_DESC;

private:
	CWeapon_ShortBow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_ShortBow(const CWeapon_ShortBow& Prototype);
	~CWeapon_ShortBow() override = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

private:
	HRESULT		Ready_Components()		override;
	//HRESULT		Bind_ShaderResources()		override;

public:
	static  CWeapon_ShortBow*	 Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)	override;
	void	Free()						override;
};

END