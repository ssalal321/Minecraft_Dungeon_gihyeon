#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

namespace Engine
{
	class CTexture;
	class CVIBuffer_Rect;
}

BEGIN(Client)
class CItem;
class CInventoryIcon;

class CInventorySlot abstract : public CUIObject
{
public:
	typedef struct INVENTORY_SLOT_DESC : public CUIObject::UIOBJECT_DESC
	{
		_float		fPlayTime;

		INVENTORY_SLOT_DESC(const _tchar* GameObjectTag, UI_STATE uiState,
			_float x, _float y, _float z, _float sizeX, _float sizeY,
			const wstring& textureTag, _float speedPerSec = 0.f, _float rotationPerSec = 0.f, _float playTime = 0.f)
			: UIOBJECT_DESC(GameObjectTag, uiState, x, y, z, sizeX, sizeY, textureTag,
				speedPerSec, rotationPerSec),  // 부모 생성자 호출
			fPlayTime(playTime) {
		}

		//// 복사 생성자
		//INVENTORY_ITEMSLOT_DESC(const INVENTORY_ITEMSLOT_DESC& other)
		//	: UIOBJECT_DESC(other), fPlayTime(other.fPlayTime) { }

		~INVENTORY_SLOT_DESC() override = default;

	}INVENTORY_SLOT_DESC;

protected:
    CInventorySlot(ID3D11Device* device, ID3D11DeviceContext* context);
    ~CInventorySlot() override;

public:
	_bool	Is_Empty() const { return m_bIsEmpty; }

	void	Set_Empty(_bool bIsEmpty) { m_bIsEmpty = bIsEmpty; }
	void	Set_IconTag(const _wstring& strIconTexPrototypeTag) { m_strIconTexPrototypeTag = strIconTexPrototypeTag; }

public:
	HRESULT		Initialize(void* pArg)		override;

public:
	HRESULT		Add_Icon_Image(const _wstring& GameObjectTag, const _wstring& strIconTexPrototypeTag);

protected:
	CTexture*			m_pEmptyTextureCom	= { nullptr };
    CTexture*           m_pSlotTextureCom	= { nullptr };
    CShader*            m_pShaderCom	= { nullptr };
    CVIBuffer_Rect*     m_pVIBufferCom	= { nullptr };

	INVENTORY_SLOT_DESC*	m_pDesc		= { nullptr };
	CInventoryIcon*			m_pIcon		= { nullptr };

	_int		m_iSlotIndex = { -1 };
	_wstring	m_strIconTexPrototypeTag = {};

	_bool		m_bIsEmpty = { true };

protected:
    //void    Create_Icon(const wstring& texPrototypeTag);

public:
    void    Free()	override;
};

END