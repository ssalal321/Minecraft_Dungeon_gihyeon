#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_ImGui final : public CLevel
{
private:
	CLevel_ImGui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	~CLevel_ImGui() override = default;

public:
	HRESULT		Initialize()						override;
	void		Update(_float fTimeDelta)			override;
	HRESULT		Render()							override;

public:
	HRESULT		Ready_Layer_UI(const _wstring& strLayerTag);

public:
	static	 CLevel_ImGui*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual  void			Free() override;
};

END