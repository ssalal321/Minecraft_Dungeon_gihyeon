#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float3				vEye = {};
		_float3				vAt = {};
		_float				fFov = {};
		_float				fNear = {};
		_float				fFar = {};
	}CAMERA_DESC;

protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& Prototype);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Last_Update(_float fTimeDelta) override;

protected:
	/* 투영변환행렬을 만들어내기위한 정보들. */
	_float				m_fFov = {};
	_float				m_fNear = {};
	_float				m_fFar = {};
	_float				m_fAspect = {};

public:
	HRESULT Update_Camera();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END