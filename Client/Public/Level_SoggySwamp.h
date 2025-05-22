#pragma once

#include <Collider.h>

#include "Client_Defines.h"
#include "Level.h"


namespace Client
{
}

BEGIN(Client)
class CPlayer;
class CMonster;
class CCauldronBoss_Trigger;
class CCauldronBoss;
class CCauldronBossHP;

class CMonsterRush_Trigger;

class CLevel_SoggySwamp final : public CLevel
{
private:
	CLevel_SoggySwamp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	~CLevel_SoggySwamp() override = default;

public:
	HRESULT		Initialize()						override;
	void		Update(_float fTimeDelta)			override;
	HRESULT		Render()							override;

public:
	HRESULT		Ready_PrePlayer();
	HRESULT		Ready_Lights();
	HRESULT		Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT		Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT		Ready_Layer_Player(const _wstring& strLayerTag);
	HRESULT		Ready_Layer_Boss(const _wstring& strLayerTag);
	HRESULT		Ready_Layer_Trigger(const _wstring& strLayerTag);
	HRESULT		Ready_Layer_MonsterRush(const _wstring& strLayerTag);
	HRESULT		Ready_Layer_UI(const _wstring& strLayerTag);


	HRESULT		Ready_BabyZombie(const _wstring& strLayerTag);
	HRESULT		Ready_Slime(const _wstring& strLayerTag);
	HRESULT		Ready_Skeleton(const _wstring& strLayerTag);
	HRESULT		Ready_Vindicator(const _wstring& strLayerTag);
	HRESULT		Ready_Zombie(const _wstring& strLayerTag);

	CCollider*	Get_Closest_Collider(const _float4& mousePos, const _float3& mouseRay);
	void		Click_Chase_Monster(CMonster* pMonster);

private:
	_bool	bShowInventory = { false };
	_bool	m_bHoveringMonster = { false };

	CPlayer*	m_pPlayer = { nullptr };
	CMonster*	m_pPickedMonster = { nullptr };

	CCauldronBoss*		m_pCauldronBoss = { nullptr };

	CCauldronBoss_Trigger*		m_pBoss_Trigger = { nullptr };
	CCauldronBossHP*			m_pCauldronBossHP = { nullptr };

	CMonsterRush_Trigger*		m_pMonsterRush_Trigger = { nullptr };

public:
	static CLevel_SoggySwamp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
