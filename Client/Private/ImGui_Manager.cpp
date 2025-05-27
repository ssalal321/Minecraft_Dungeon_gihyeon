#include "ImGui_Manager.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ImGuiFileDialog.h"

IMPLEMENT_SINGLETON(CImGui_Manager)

HRESULT CImGui_Manager::Initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context)
{
	// Setup Dear IMGUI context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	// Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	// Enable Gamepad Controls

	// Setup Dear ImGui
	// style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, context);

	return S_OK;
}

void CImGui_Manager::Bind()
{
	// ImGui 시작

	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	/*			ImGui 창 구현		*/
	//// Demo 창 구현
	//ImGui::ShowDemoWindow();

	ImGui::Begin("My Inventory Tool");
	ImGui::Text("Debugging Info");
	//ImGui::Checkbox("Show Stats", &m_bShowStats);
	ImGui::End();

	ImGui::Render();
}

void CImGui_Manager::Render()
{
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImGui_Manager::Release_ImGui()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	DestroyInstance();
}

void CImGui_Manager::Free()
{
	__super::Free();
}
