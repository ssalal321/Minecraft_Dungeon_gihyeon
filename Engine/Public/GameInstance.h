#pragma once

/* 클라이언트와 엔진 사이에 기능적인 소통을 위한 클래스. */
/* 엔진의 기능(함수)을 클라이언트에 보여주기 위한 클래스.  */

#include "Renderer.h"
#include "Prototype_Manager.h"
#include "PipeLine.h"
#include "UI_Manager.h"

BEGIN(Engine)
	class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	~CGameInstance() override = default;

public:
	HRESULT		Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void		Update_Engine(_float fTimeDelta);
	HRESULT		Draw();
	void		Clear(_uint iCurrentLevelIndex, _uint iNextLevelIndex);

public:
	_float		Compute_Random_Normal();
	_float		Compute_Random(_float fMin, _float fMax);

#pragma region GRAPHIC_DEVICE
	HRESULT		Clear_BackBuffer_View(_float4 vClearColor);	
	HRESULT		Clear_DepthStencil_View();
	HRESULT		Present();
#pragma endregion

#pragma region INPUT_DEVICE
	_byte		Get_DIKeyState(_ubyte byKeyID);
	_byte		Get_DIMouseState(MOUSEKEYSTATE eMouse);	
	_long		Get_DIMouseMove(MOUSEMOVESTATE eMouseState);
#pragma endregion

#pragma region TIMER_MANAGER
public:
	_float		Get_TimeDelta(const _wstring& strTimerTag);
	HRESULT		Add_Timer(const _wstring& strTimerTag);
	void		Compute_TimeDelta(const _wstring& strTimerTag);
#pragma endregion

#pragma region LEVEL_MANAGER
	HRESULT		Open_Level(_uint iNextLevelIndex, class CLevel* pNewLevel);
#pragma endregion

#pragma region PROTOTYPE_MANAGER
	HRESULT		Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype);
	CBase*		Clone_Prototype(PROTOTYPE ePrototype, _uint iPrototypeLevelIndex, _wstring strPrototypeTag, void* pArg = nullptr);
#pragma endregion

#pragma region OBJECT_MANAGER
	HRESULT		Add_GameObject(_uint iPrototypeLevelIndex, _wstring strPrototypeTag,
							   _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr);
	CGameObject* Find_GameObject(_wstring strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag);
#pragma endregion

#pragma region INPUT_MANAGER
	_bool		Get_Key(_int _iKey) const; //현재 프레임에 눌렸는지 여부
	_bool		Key_Pressing(_int _iKey) const; //계속 눌리는 중일 때
	_bool		Key_Down(_int _iKey) const; //처음 눌릴 때
	_bool		Key_Up(_int _iKey) const; //처음 눌리지 않을 때
	_float3		Get_MousePos() const;
#pragma endregion

#pragma region PICKING
	void	Compute_MouseRay(_float3& worldMousePos, _float3& worldMouseRay) const;
#pragma endregion

#pragma region RENDERER
	HRESULT		Add_RenderObject(CRenderer::RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
#pragma endregion

#pragma region PIPELINE
	const	_float4x4*	Get_Transform_Float4x4(CPipeLine::TRANSFORMSTATE eState) const;
	const	_matrix		Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE eState) const;
	const	_float4x4*	Get_Transform_Inverse_Float4x4(CPipeLine::TRANSFORMSTATE eState);
	const	_matrix		Get_Transform_Inverse_Matrix(CPipeLine::TRANSFORMSTATE eState) const;
	const	_float4*	Get_CamPosition() const;
	void				Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix StateMatrix);
#pragma endregion

#pragma region LIGHT_MANAGER
	const LIGHT_DESC*	Get_LightDesc(_uint iIndex) const;
	HRESULT				Add_Light(const LIGHT_DESC& LightDesc);
#pragma endregion

#pragma region UI_MANAGER
	CUIObject*	Add_UIObject(_uint iPrototypeLevelIndex, _uint iLayerLevelIndex, const _wstring& strPrototypeTag, CUI_Manager::UI_LIFETIME eUILifeTime, void* pArg = nullptr);
#pragma endregion


private:
	class	CGraphic_Device*		m_pGraphic_Device		= { nullptr };
	class	CInput_Device*			m_pInput_Device			= { nullptr };
	class	CTimer_Manager*			m_pTimer_Manager		= { nullptr };	
	class	CLevel_Manager*			m_pLevel_Manager		= { nullptr };
	class	CPrototype_Manager*		m_pPrototype_Manager	= { nullptr };
	class	CObject_Manager*		m_pObject_Manager		= { nullptr };
	class   CInput_Manager*			m_pInput_Manager		= { nullptr };
	class	CRenderer*				m_pRenderer				= { nullptr };
	class	CPipeLine*				m_pPipeLine				= { nullptr };
	class	CLight_Manager*			m_pLight_Manager		= { nullptr };
	class   CUI_Manager*			m_pUI_Manager			= { nullptr };
	class	CPicking*				m_pPicking				= { nullptr };
	
public:
	void	Release_Engine();
	void	Free()				override;
};

END