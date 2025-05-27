#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)

class CImGui_Manager final: public CBase
{
	DECLARE_SINGLETON(CImGui_Manager)
private:
	CImGui_Manager() = default;
	~CImGui_Manager() override = default;

public:
	HRESULT Initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context);
	void	Bind();
	void	Render();
	void	Release_ImGui();
	void	Free() override;
};

END