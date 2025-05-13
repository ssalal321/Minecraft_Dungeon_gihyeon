#pragma once
#include "Client_Defines.h"
#include "Item.h"

BEGIN(Client)

class CArmor abstract : public CItem
{
public:
	typedef struct tagArmorDesc : ITEM_DESC
	{

	}ARMOR_DESC;

protected:
	CArmor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CArmor(const CArmor& Prototype);
	~CArmor() override = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

protected:
	map<const _wstring, class CPartObject*>		m_ArmorPartObjects;

protected:
	HRESULT		Ready_Components()		override;
	virtual  HRESULT     Ready_Armor_PartObjects() = 0;

	CPartObject*	Find_PartObject(const _wstring& strPartObjectTag);
	HRESULT			Add_PartObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strPartObjectTag, void* pArg = nullptr);
	
	//HRESULT		Bind_ShaderResources()		override;

public:
	void	Free()						override;
};

END