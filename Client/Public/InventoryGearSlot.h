#pragma once
#include "Client_Defines.h"
#include "InventorySlot.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CInventoryGearSlot final : public CInventorySlot
{
private:
	CInventoryGearSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInventoryGearSlot(const CInventoryGearSlot& Prototype);
	~CInventoryGearSlot() override = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;

	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;

	HRESULT		Render()							override;

private:
	GEAR_TYPE	m_eGearType = { GEAR_TYPE::NONE };

private:
	HRESULT		Ready_Components();


public:
	static	 CInventoryGearSlot*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)	override;
	void			Free()				override;
};

END