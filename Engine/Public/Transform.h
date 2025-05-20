#pragma once

#include "Component.h"
#include "Navigation.h"

/* 월드변환을 위한 행렬을 보관한다.                     */
/* 월드공간에서의 객체의 상태변환을 위한 함수를 제공한다.  */
/* 월드행렬을 현재 객체의 상태에 맞게 직접 갱신한다.      */

/* XMLoadxxxxx -> 저장용 타입에서 연산용타입으로 치환.  */
/* XMStorexxxxx -> 연산용타입에서 저장용 타입으로 저장. */

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION };

	typedef struct tagTransformDesc
	{
		_float fRotationPerSec;
		_float fSpeedPerSec;

		// 부모 클래스 멤버를 한 번에 초기화할 수 있도록 하위 클래스 생성자에서 받음
		tagTransformDesc(_float rotationPerSec = 0.f, _float speedPerSec = 0.f)
			: fRotationPerSec(rotationPerSec), fSpeedPerSec(speedPerSec) {}

		/*tagTransformDesc(const tagTransformDesc &other)
			:fRotationPerSec(other.fRotationPerSec), fSpeedPerSec(other.fSpeedPerSec) {}*/

		virtual ~tagTransformDesc() = default;

	}TRANSFORM_DESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	~CTransform() override = default;

public:
	_vector Get_State(STATE eState) const
	{
		return XMLoadFloat4x4(&m_WorldMatrix).r[eState];
	}

	_matrix Get_WorldMatrix_Inverse() const
	{
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}

	const _float4x4& Get_WorldMatrix() const
	{
		return m_WorldMatrix;
	}

	_float4x4* Get_WorldMatrix_Ptr()
	{
		return &m_WorldMatrix;
	}

	_bool	Get_Is_Jumping() const { return m_bIsJumping; }

	_float	Get_Rotation_perSec() const { return m_fRotationPerSec; }

	void	Set_Is_Jumping(_bool bJumping) { m_bIsJumping = bJumping; }

	void	Set_State(STATE eState, _fvector vState)
	{
		XMStoreFloat4(reinterpret_cast<_float4*>(&m_WorldMatrix.m[eState][0]), vState);
	}

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;	

public:
	HRESULT		Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);

public:
	_float3		Compute_Scaled();
	void		SetUp_Scale(_float fScaleX = 1.f, _float fScaleY = 1.f, _float fScaleZ = 1.f);

public:
	void	Go_Straight(_float fTimeDelta, CNavigation* pNavigation = nullptr, _float fSpeedFactor = 1.f);
	void	Go_Left(_float fTimeDelta);
	void	Go_Right(_float fTimeDelta);
	void	Go_Backward(_float fTimeDelta);
	void	Jump_Start(_float fJumpVelocity);
	void	Jump(_float fTimeDelta, CNavigation* pNavigation = nullptr);

	void	Turn(_fvector vAxis, _float fTimeDelta);
	void	Turn_Around_Offset(_fvector vAxis, _float fRadian, _float fOffsetDistance);
	void	Rotation(_fvector vAxis, _float fRadian);
	void	LookAt(_fvector vAt);

public:
	void	Add_Momentum(_vector vImpulse);
	void	Update_Momentum(_float fTimeDelta);

	void	Start_BezierFlight(const XMFLOAT3& vStart, const XMFLOAT3& vControl, const XMFLOAT3& vEnd, _float fSpeed);
	void	Update_BezierFlight(_float fDeltaTime);

private:
	/* row_major = Right, Up, Look, Position */
	_float4x4	m_WorldMatrix = {};

	_float		m_fSpeedPerSec = { };
	_float		m_fRotationPerSec = { };

	_bool		m_bIsJumping	= { false };    // 점프 중인지 여부
	_float		m_fJumpVelocity = {};			// 현재 y축 속도(점프 속도)
	_float		m_fGravity		= { -9.8f };    // 중력 가속도 (m/s²)
	_float		m_fCurrentY		= {};			// 현재 Y 위치

	_float3		m_Velocity = { 0.f, 0.f, 0.f };

	_vector		m_vJumpDirection = {};

#pragma region BEZIER CURVE
	_bool       m_bBezierFlying = { false };
	_float      m_fBezierT		= { 0.f };
	_float      m_fBezierSpeed	= { 1.f };

	_float3     m_vBezierStart	  = {};
	_float3     m_vBezierControl  = {};
	_float3     m_vBezierEnd	  = {};
#pragma endregion

public:
	static	  CTransform*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual   CComponent*	Clone(void* pArg) override;
	virtual   void			Free() override;
};

END