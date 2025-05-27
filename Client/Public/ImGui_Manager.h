#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)

class CImGui_Manager final: public CBase
{
public:
    struct UIElementData
    {
        std::string        name;
        std::string        texturePath;
        ID3D11ShaderResourceView* pTextureSRV;
        ImTextureID     imguiTexture;
        ImVec2          position;
        ImVec2          size;
        _float          alpha;

        // 기본 생성자
        UIElementData()
            : name("UI_Element")
            , texturePath("")
            , pTextureSRV(nullptr)
            , imguiTexture(static_cast<ImTextureID>(0))
            , position({ 100, 100 })
            , size({ 200, 100 })
            , alpha(1.0f)
        {
        }

        // 사용자 지정 생성자 (emplace_back 용)
        UIElementData(const std::string& name, const std::string& path, ImVec2 pos, ImVec2 size, _float alpha = 1.0f)
            : name(name)
            , texturePath(path)
            , pTextureSRV(nullptr)
            , imguiTexture(static_cast<ImTextureID>(0))
            , position(pos)
            , size(size)
            , alpha(alpha)
        {
        }
    };


    DECLARE_SINGLETON(CImGui_Manager)
private:
	CImGui_Manager() = default;
	~CImGui_Manager() override = default;

private:
    std::map<std::string, std::vector<UIElementData>> m_UIFolderMap;
    char    m_szFolderPath[260] = "Textures\\UI";

    ID3D11Device*           m_pDevice = nullptr;
    ID3D11DeviceContext*    m_pContext = nullptr;

private:
    void    Show_Multi_UIEditor();
     void   Load_Textures_From_Folder(const std::string& folder);
    _bool   Load_Texture_From_File(const std::string& path, ID3D11ShaderResourceView** outSRV);
    ID3D11ShaderResourceView* Create_Fallback_Purple_Texture();

public:
	HRESULT Initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context);
	void	Bind();
	void	Render();
	void	Release_ImGui();
	void	Free() override;
};

END