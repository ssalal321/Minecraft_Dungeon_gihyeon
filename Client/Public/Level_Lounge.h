#pragma once

#include <Collider.h>

#include "Client_Defines.h"
#include "Level.h"

namespace Client
{
	class CLevel_Trigger;
}

BEGIN(Client)
	class CPlayer;
	class CMonster;

class CLevel_Lounge final : public CLevel
{
private:
	CLevel_Lounge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	~CLevel_Lounge() override = default;

public:
	HRESULT		Initialize()						override;
	void		Update(_float fTimeDelta)			override;
	HRESULT		Render()							override;

public:
	HRESULT		Ready_Lights();
	HRESULT		Ready_Layer_Camera(const _wstring& strLayerTag);
	
	HRESULT		Ready_Layer_Player(const _wstring& strLayerTag);
	HRESULT		Ready_Layer_Monster(const _wstring& strLayerTag);
	HRESULT		Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT		Ready_Layer_InventoryUI(const _wstring& strLayerTag);
	HRESULT		Ready_Layer_PlayerSlotUI(const _wstring& strLayerTag);


private:
	_bool	bShowInventory = { false };
	
	CLevel_Trigger*		m_pLevel_Trigger = { nullptr };

	CPlayer*	m_pPlayer = { nullptr };
	

public:
	static CLevel_Lounge* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END