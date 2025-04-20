#pragma once

#include <Collider.h>

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)
	class CPlayer;
	class CMonster;

class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	~CLevel_GamePlay() override = default;

public:
	HRESULT		Initialize()						override;
	void		Update(_float fTimeDelta)			override;
	HRESULT		Render()							override;

public:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Player(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const _wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT Ready_Layer_UI(const _wstring& strLayerTag);

	CCollider*	Get_Closest_Collider(const _float4& mousePos, const _float3& mouseRay);
	void		Handle_Monster_Click(CMonster* pMonster);

private:
	_bool	bShowInventory = { false };
	_bool   m_bClickLock = { false };
	_bool	m_bHoveringMonster = { false };

	CPlayer*	m_pPlayer = { nullptr };
	CMonster*	m_pPickedMonster = { nullptr };

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END