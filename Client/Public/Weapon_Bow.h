#pragma once
#include "Client_Defines.h"
#include "Item.h"

BEGIN(Client)

class CWeapon_Bow final : public CItem
{
public:
	typedef struct tagWeaponBowDesc : ITEM_DESC
	{


	}WEAPON_BOW_DESC;

private:
	CWeapon_Bow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Bow(const CWeapon_Bow& Prototype);
	~CWeapon_Bow() override = default;

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
	static  CWeapon_Bow*	 Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)	override;
	void	Free()						override;
};

END