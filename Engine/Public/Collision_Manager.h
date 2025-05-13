#pragma once
#include <unordered_set>

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
	HRESULT		Add_ColliderCom(_uint iLevelIndex, CComponent* pColliderCom, const _wstring& ColliderTag, const _wstring& ObjectType, _bool bPersistent = false);
    HRESULT		Attach_Persistent_Colliders_To_Level(_uint iLevelIndex, const wstring& targetTag);
    unordered_map<_wstring, vector<CCollider*>>*	Get_Colliders(_uint iLevelIndex);
	void		Resolve_Penetration_And_Slide(CCollider* pColA, CCollider* pColB, _float fForce);

public:
    HRESULT     Initialize(_uint iNumLevels);
    void        Update();

#ifdef _DEBUG
    HRESULT     Render();
#endif

    void        Clear(_uint iCurrentLevelIndex);

private:
	class CGameInstance*	    m_pGameInstance = { nullptr };

	unordered_map<_wstring, vector<CCollider*>>*	m_pColliderGroups			= { nullptr };
	unordered_map<_wstring, vector<CCollider*>>*	m_pPersistentColliderGroups = { nullptr }; // 레벨에 관계없이 살아남음

	// 어떤 Collider 그룹(tag)이 Persistent인지 추적하기 위한 Set
	unordered_set<wstring> m_PersistentTags;

private:
    _uint   m_iNumLevels = {};

public:
	static	 CCollision_Manager*	Create(_uint iNumLevels);
	void	 Free() override;
};
END