#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CItem;
class CInventorySlot;
class CInventoryArtifactSlot;
class CInventoryGearSlot;
class CInventoryStoreSlot;

class CInventoryBase final : public CUIObject
{
public:
	typedef struct INVENTORY_BASE_DESC : public CUIObject::UIOBJECT_DESC
	{
		_float			fPlayTime;

		INVENTORY_BASE_DESC(const _tchar* GameObjectTag, UI_STATE uiState,
			_float x, _float y, _float z, _float sizeX, _float sizeY,
			const wstring& textureTag, _bool alphaBlend = false, _float speedPerSec = 0.f, _float rotationPerSec = 0.f, _float playTime = 0.f)
			: UIOBJECT_DESC(GameObjectTag, uiState, x, y, z, sizeX, sizeY, textureTag, alphaBlend,
							rotationPerSec, speedPerSec),  // 부모 생성자 호출
			  fPlayTime(playTime) { }

		//// 복사 생성자
		//INVENTORY_BASE_DESC(const INVENTORY_BASE_DESC& other)
		//	: UIOBJECT_DESC(other), fPlayTime(other.fPlayTime) { }

		~INVENTORY_BASE_DESC() override = default;

	}INVENTORY_BASE_DESC;

private:
	CInventoryBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInventoryBase(const CInventoryBase& Prototype);
	~CInventoryBase() override = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;

	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;

	HRESULT		Render()							override;

private:
	INVENTORY_BASE_DESC*	m_pDesc		= { nullptr };

	CTexture*			m_pTextureCom	= { nullptr };
	CShader*			m_pShaderCom	= { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom	= { nullptr };

	vector<CInventoryStoreSlot*>		m_UIStoreSlots;
	vector<CInventoryGearSlot*>			m_UIGearSlots;       // 고정 크기 3
	vector<CInventoryArtifactSlot*>		m_UIArtifactSlots;   // 고정 크기 3

	_bool		m_bReadyForEvents = { false };

private:
	HRESULT		Ready_Subscribe_Events();
	HRESULT		Ready_UISlots();
	HRESULT		Ready_Components();

	void		Add_Icon_To_StoreSlot(const Item_Added_To_StoreSlot& itemAddedEvent);
	void		Unequip_Icon_To_StoreSlot(const Unequipped_To_StoreSlot& event);
	void		Swap_Store_and_Gear_Icon(const Swap_Store_with_Gear& swap_Store_GearEvent);
	void		Swap_Store_and_Artifact_Icon(const Swap_Store_with_Artifact& swap_Store_ArtifactEvent);
	void		Equip_Icon_To_GearSlot(const Equip_To_Gear& equip_To_GearEvent);
	void		Equip_Icon_To_ArtifactSlot(const Equip_To_Artifact& equip_To_ArtifactEvent);

public:
	static	 CInventoryBase*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)	override;
	void			Free()				override;
};

END
