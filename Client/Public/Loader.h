#pragma once

/* 내가 지정한 레벨의 대한 자원을 준비한다. */
/* 지정한레벨을 할당할때 필요한 자원을 준비한다. */
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	HRESULT Initialize(LEVEL eNextLevelID);
	HRESULT Loading();
	void Show_LoadingText();
	_bool isFinished();
	

private:
	LEVEL					m_eNextLevelID = { LEVEL_END };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };

private:
	_tchar					m_szLoadingText[MAX_PATH] = {};
	_bool					m_isFinished = { false };

private:
	HANDLE					m_hThread = {};
	CRITICAL_SECTION		m_Critical_Section = {  };


private:
	HRESULT Loading_For_Logo();
	HRESULT Loading_For_GamePlay();


public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free() override;
};

END