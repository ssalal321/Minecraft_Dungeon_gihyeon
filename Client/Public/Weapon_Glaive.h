#pragma once
#include "Client_Defines.h"
#include "Item.h"

BEGIN(Client)

class CWeapon_Glaive final : public CItem
{
public:
	typedef struct tagWeaponGlaiveDesc : ITEM_DESC
	{


	}WEAPON_GLAIVE_DESC;

private:
	CWeapon_Glaive(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Glaive(const CWeapon_Glaive& Prototype);
	~CWeapon_Glaive() override = default;

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
	static  CWeapon_Glaive*	 Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)	override;
	void	Free()						override;
};

END