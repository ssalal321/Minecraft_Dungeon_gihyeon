#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Title final : public CLevel
{
private:
	CLevel_Title(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	~CLevel_Title() override = default;

public:
	HRESULT		Initialize()						override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

public:
	HRESULT		Ready_Layer_BackGround(const _wstring& strLayerTag);

public:
	static	 CLevel_Title*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual  void			Free() override;
};

END