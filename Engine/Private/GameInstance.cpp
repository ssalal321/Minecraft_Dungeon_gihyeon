#include "GameInstance.h"

#include "Input_Device.h"
#include "Graphic_Device.h"
#include "Timer_Manager.h"
#include "Level_Manager.h"
#include "Prototype_Manager.h"
#include "Object_Manager.h"
#include "Input_Manager.h"
#include "Renderer.h"
#include "Light_Manager.h"
#include "UI_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.isWindowed, EngineDesc.iViewportWidth, EngineDesc.iViewportHeight, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pInput_Device = CInput_Device::Create(EngineDesc.hInstance, EngineDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pPrototype_Manager = CPrototype_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pPrototype_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pInput_Manager = CInput_Manager::Create(EngineDesc.hWnd);
	if (nullptr == m_pInput_Manager)
		return E_FAIL;

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	m_pUI_Manager = CUI_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Update_Engine(_float fTimeDelta)
{
	m_pInput_Device->Update();

	m_pInput_Manager->Update_Key();

	m_pObject_Manager->Priority_Update(fTimeDelta);
	m_pUI_Manager->Priority_Update(fTimeDelta);
		
	m_pObject_Manager->Update(fTimeDelta);
	m_pUI_Manager->Update(fTimeDelta);

	m_pPipeLine->Update();

	m_pObject_Manager->Last_Update(fTimeDelta);
	m_pUI_Manager->Late_Update(fTimeDelta);

	m_pLevel_Manager->Update(fTimeDelta);
}

HRESULT CGameInstance::Draw()
{
	m_pRenderer->Draw();

	m_pUI_Manager->Render_UI();

	m_pLevel_Manager->Render();

	return S_OK;
}

void CGameInstance::Clear(_uint iCurrentLevelIndex, _uint iNextLevelIndex)
{
	/* 지정한 레벨용 자원을 파괴한다. */
	m_pObject_Manager->Clear(iCurrentLevelIndex);

	m_pUI_Manager->Clear(iCurrentLevelIndex, iNextLevelIndex);

	m_pPrototype_Manager->Clear(iCurrentLevelIndex);

}

_float CGameInstance::Compute_Random_Normal()
{
	return rand() / static_cast<_float>(RAND_MAX);
}

_float CGameInstance::Compute_Random(_float fMin, _float fMax)
{
	return	fMin + (fMax - fMin) * Compute_Random_Normal();
}


#pragma region GRAPHIC_DEVICE
HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	return m_pGraphic_Device->Present();
}
#pragma endregion


#pragma region INPUT_DEVICE

_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyState(byKeyID);
}
_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_DIMouseState(eMouse);
}
_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}
#pragma endregion


#pragma region TIMER_MANAGER
_float CGameInstance::Get_TimeDelta(const _wstring& strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.f;

	return m_pTimer_Manager->Get_TimeDelta(strTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _wstring& strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

void CGameInstance::Compute_TimeDelta(const _wstring& strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return;

	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}
#pragma endregion


#pragma region LEVEL_MANAGER
HRESULT CGameInstance::Open_Level(_uint iNextLevelIndex, CLevel* pNewLevel)
{
	return m_pLevel_Manager->Open_Level(iNextLevelIndex, pNewLevel);
}
#pragma endregion


#pragma region PROTOTYPE_MANAGER
HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype)
{
	return m_pPrototype_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

CBase* CGameInstance::Clone_Prototype(PROTOTYPE ePrototype, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	return m_pPrototype_Manager->Clone_Prototype(ePrototype, iLevelIndex, strPrototypeTag, pArg);
}
#pragma endregion


#pragma region OBJECT_MANAGER
HRESULT CGameInstance::Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	return m_pObject_Manager->Add_GameObject(iPrototypeLevelIndex, strPrototypeTag, iLayerLevelIndex, strLayerTag, pArg);
}
#pragma endregion


#pragma region INPUT_MANAGER
_bool CGameInstance::Get_Key(_int _iKey) const
{
	return m_pInput_Manager->Get_Key(_iKey);
}

_bool CGameInstance::Key_Pressing(_int _iKey) const
{
	return m_pInput_Manager->Key_Pressing(_iKey);
}

_bool CGameInstance::Key_Down(_int _iKey) const
{
	return m_pInput_Manager->Key_Down(_iKey);
}

_bool CGameInstance::Key_Up(_int _iKey) const
{
	return m_pInput_Manager->Key_Up(_iKey);
}

_float3 CGameInstance::Get_MousePos() const
{
	return m_pInput_Manager->Get_MousePos();
}
#pragma endregion


#pragma region RENDERER
HRESULT CGameInstance::Add_RenderObject(CRenderer::RENDERGROUP eRenderGroup, CGameObject* pRenderObject)
{
	return m_pRenderer->Add_RenderObject(eRenderGroup, pRenderObject);
}

const _float4x4* CGameInstance::Get_Transform_Float4x4(CPipeLine::TRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Float4x4(eState);
}

const _matrix CGameInstance::Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Matrix(eState);
}

const _float4x4* CGameInstance::Get_Transform_Inverse_Float4x4(CPipeLine::TRANSFORMSTATE eState)
{
	return m_pPipeLine->Get_Transform_Inverse_Float4x4(eState);
}

const _matrix CGameInstance::Get_Transform_Inverse_Matrix(CPipeLine::TRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Inverse_Matrix(eState);
}

const _float4* CGameInstance::Get_CamPosition() const
{
	return m_pPipeLine->Get_CamPosition();
}

void CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix StateMatrix)
{
	m_pPipeLine->Set_Transform(eState, StateMatrix);
}
#pragma endregion


#pragma region LIGHT_MANAGER
const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint iIndex) const
{
	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
	return m_pLight_Manager->Add_Light(LightDesc);
}
#pragma endregion


#pragma region UI_MANAGER
HRESULT CGameInstance::Add_UIObject(_uint iPrototypeLevelIndex, _uint iCurrentLevelIndex, const _wstring& strPrototypeTag, CUI_Manager::UI_LIFETIME eUILifeTime, void* pArg)
{
	return m_pUI_Manager->Add_UIObject(iPrototypeLevelIndex, iCurrentLevelIndex, strPrototypeTag, eUILifeTime, pArg);
}

void CGameInstance::Set_UIObject_Callback(CUIObject* pUIObject, std::function<void()> callback)
{
	m_pUI_Manager->Set_UIObject_Callback(pUIObject, callback);
}

#pragma endregion


void CGameInstance::Release_Engine()
{
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pPrototype_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pInput_Manager);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pUI_Manager);


	DestroyInstance();
}

void CGameInstance::Free()
{
	__super::Free();


}
