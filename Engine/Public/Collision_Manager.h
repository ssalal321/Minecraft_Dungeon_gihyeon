#pragma once
#include "Base.h"

BEGIN(Engine)
	class CComponent;
	class CCollider;

class CCollision_Manager final : public CBase
{
private:
    CCollision_Manager();
	~CCollision_Manager() override = default;

public:
    HRESULT     Initialize(_uint iNumLevels);

	HRESULT		Add_ColliderCom(CComponent* pColliderCom/*, _wstring OwnerSortTag*/);

    void        Update();

#ifdef _DEBUG
    HRESULT     Render();
#endif

    void        Clear(_uint iCurrentLevelIndex);

private:
	class CGameInstance*	    m_pGameInstance = { nullptr };
	vector<CCollider*>			m_pColliders;

private:
    _uint   m_iNumLevels = {};

public:
	static	 CCollision_Manager*	Create(_uint iNumLevels);
	void	 Free() override;
};
END