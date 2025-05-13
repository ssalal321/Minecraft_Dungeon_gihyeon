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
	~CLoader() override = default;

public:
	HRESULT		Initialize(LEVEL eNextLevelID);
	HRESULT		Loading();
	void		Show_LoadingText();
	_bool		Is_Finished();
	

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
	HRESULT		Loading_For_Static();
	HRESULT		Loading_For_Title();
	HRESULT		Loading_For_Lounge();
	HRESULT		Loading_For_SoggySwamp();


	HRESULT		Ready_Prototype_TextureCom_Static();
	HRESULT		Ready_Prototype_ShaderCom_Static();
	HRESULT		Ready_Prototype_ModelCom_Static();
	HRESULT		Ready_Prototype_ColliderCom_Static();
	HRESULT		Ready_Prototype_GameObject_Static();


	HRESULT		Ready_Prototype_Armors_Static();

public:
	static	 CLoader*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	void	Free()	override;
};

END