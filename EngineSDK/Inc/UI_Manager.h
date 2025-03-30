#pragma once
#include "Base.h"

BEGIN(Engine)
	class CUIObject;

	class CUI_Manager : public CBase
{
public:
    enum UI_LIFETIME { TEMPORARY, PERSISTENT, LIFETIME_END  };

private:
	CUI_Manager();
	~CUI_Manager() override = default;

public:
    HRESULT     Initialize(_uint iNumLevels);

    CUIObject*   Add_UIObject(_uint iPrototypeLevelIndex, _uint iLayerLevelIndex, const _wstring& strPrototypeTag,
							 UI_LIFETIME eUILifeTime, void* pArg = nullptr);

    void        Priority_Update(_float fDeltaTime);
    void        Update(_float fDeltaTime);
    void        Late_Update(_float fDeltaTime);

    HRESULT     Render_UI();
    void        Clear(_uint iCurrentLevelIndex, _uint iNextLevelIndex);
   

private:
    _uint   m_iObjectLevelIndex     = {};  // 현재 레벨의 UI 객체 인덱스

	class CGameInstance*	    m_pGameInstance = { nullptr };

    std::vector<CUIObject*>     m_CurrentUIObjects[LIFETIME_END]   = {};  // 관리할 UI 객체들
    std::vector<CUIObject*>     m_NextUIObjects[LIFETIME_END] = {};       // 관리할 UI 객체들

private:
    _uint   m_iNumLevels = {};

public:
	static	 CUI_Manager*	Create(_uint iNumLevels);  // 디바이스랑 컨텍스트 필요하려나..?
	void	 Free() override;
};
END