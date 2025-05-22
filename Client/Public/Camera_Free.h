#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Free final : public CCamera
{
public:
	typedef struct tagCameraFreeDesc : public CCamera::CAMERA_DESC
	{
		_float			fKeySensor = {};
	}CAMERA_FREE_DESC;

private:
	CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Free(const CCamera_Free& Prototype);
	~CCamera_Free() override = default;

public:
	HRESULT		Initialize_Prototype() override;
	HRESULT		Initialize(void* pArg) override;
	void		Priority_Update(_float fTimeDelta) override;
	void		Update(_float fTimeDelta) override;
	void		Late_Update(_float fTimeDelta) override;
	HRESULT		Render() override;

private:
	_bool		m_bMouseFree = { false };
	_float		m_fKeySensor = { };

public:
	static CCamera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END

