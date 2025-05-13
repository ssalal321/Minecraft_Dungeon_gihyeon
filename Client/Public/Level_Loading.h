#pragma once

#include "Client_Defines.h"
#include "Level.h"

/* 내가 지정한 레벨의 대한 자원을 준비한다. */
/* 지정한레벨을 할당할때 필요한 자원을 준비한다. */
/* 위의 작업을 담당하는 CLoader객체를 생성해준다. */

/* 자원을 로딩하는 과정 중에서도 화면에 로딩과정을 보여준다. */

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	~CLevel_Loading() override = default;

public:
	HRESULT		Initialize(LEVEL eNextLevelID);
	void		Update(_float fTimeDelta)			override;
	HRESULT		Render()							override;
private:
	LEVEL				m_eNextLevelID = { LEVEL_END };
	class  CLoader*		m_pLoader = { nullptr };

private:
	HRESULT		Ready_Layer_BackGround();

public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free() override;
};

END