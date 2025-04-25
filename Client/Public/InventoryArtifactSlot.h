#pragma once
#include "Client_Defines.h"
#include "InventorySlot.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CInventoryArtifactSlot final : public CInventorySlot
{
private:
	CInventoryArtifactSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInventoryArtifactSlot(const CInventoryArtifactSlot& Prototype);
	~CInventoryArtifactSlot() override = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;

	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;

	HRESULT		Render()							override;

private:
	HRESULT		Ready_Components();

public:
	static	 CInventoryArtifactSlot*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)	override;
	void			Free()				override;
};

END