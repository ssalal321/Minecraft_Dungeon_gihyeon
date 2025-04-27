#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN (Client)

class CArrow : public CGameObject
{
private:
	CArrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CArrow(const CArrow& Prototype);
	~CArrow() override = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg = nullptr)	override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

public:
	void	Shoot(_float3 startPos, _float3 lookPos, _float fTimeDelta);
	void	Reset(); // 풀로 돌아갈 때 초기화할 함수

private:
	_bool	m_bActive = { false };
	GAMEOBJECT_DESC*	m_pArrowDesc = { nullptr };;

	static _int		m_iArrowID;

public:
	static CArrow*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)	override;
	void	Free()					override;
};

END