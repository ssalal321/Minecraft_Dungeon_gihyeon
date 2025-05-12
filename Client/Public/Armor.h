#pragma once
#include "Client_Defines.h"
#include "Item.h"

BEGIN(Client)

class CArmor abstract : public CItem
{
public:
	typedef struct tagArmorDesc : ITEM_DESC
	{
		
		/*const _float4x4*	pBodySocketMatrix = { nullptr };
		const _float4x4*	pL_ArmSocketMatrix = { nullptr };
		const _float4x4*	pR_ArmSocketMatrix = { nullptr };
		const _float4x4*	pL_LegSocketMatrix = { nullptr };
		const _float4x4*	pR_LegSocketMatrix = { nullptr };*/


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
	CModel*		m_pBody_ModelCom  = { nullptr };
	CModel*		m_pL_Arm_ModelCom = { nullptr };
	CModel*		m_pR_Arm_ModelCom = { nullptr };
	CModel*		m_pL_Leg_ModelCom = { nullptr };
	CModel*		m_pR_Leg_ModelCom = { nullptr };
	
	const _float4x4*	m_pBodySocketMatrix = { nullptr };
	const _float4x4*	m_pL_ArmSocketMatrix = { nullptr };
	const _float4x4*	m_pR_ArmSocketMatrix = { nullptr };
	const _float4x4*	m_pL_LegSocketMatrix = { nullptr };
	const _float4x4*	m_pR_LegSocketMatrix = { nullptr };

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