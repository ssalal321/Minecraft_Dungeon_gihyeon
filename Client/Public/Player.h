#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

/* 플레이어라는 객체를 구성하기위한 파츠들을 모아서 쥐고 있는 객체. */

BEGIN(Engine)

END

BEGIN(Client)

class CPlayer final : public CContainerObject
{
public:
	enum STATE {
		STATE_IDLE = 0x00000001,	 /* 0001 */
		STATE_RUN = 0x00000002,		 /* 0010 */
		STATE_ATTACK = 0x00000004,   /* 0100 */
		STATE_END = 0
	};
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Last_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:	
	_uint			m_iState = { STATE_END };
	

private:
	HRESULT Ready_Components();	
	HRESULT Ready_PartObjects();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END