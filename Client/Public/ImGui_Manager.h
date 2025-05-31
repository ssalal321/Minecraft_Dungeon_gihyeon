#pragma once
#include "Base.h"
#include "Client_Defines.h"

namespace Engine
{
	class CGameInstance;
}

BEGIN(Client)
	class CImGui_Manager final: public CBase
{
public:
    struct UIElementData
    {
        std::string name;
        std::string texturePath;
        ID3D11ShaderResourceView* pTextureSRV; // 실제 GPU 리소스
        ImTextureID imguiTexture;              // ImGui 디버그용 핸들
        ImVec2 position;
        ImVec2 size;
        float alpha;

        UIElementData()
            : name("UI_Element")
            , texturePath("")
            , pTextureSRV(nullptr)
            , imguiTexture(static_cast<ImTextureID>(0))
            , position({ 100.f, 100.f })
            , size({ 128.f, 128.f })
            , alpha(1.0f)
        {
        }
    };


    struct UIScreenInstance
    {
        std::string tag;
        std::string texturePath;
        ID3D11ShaderResourceView* pSRV; // GPU 리소스
        ImTextureID imguiTexture;       // 디버그용 ImGui 핸들
        ImVec2 position;
        ImVec2 size;
        float alpha;
        int layer;

        /*UIScreenInstance()
            : pSRV(nullptr)
            , imguiTexture(static_cast<ImTextureID>(0))
            , position(0.f, 0.f)
            , size(128.f, 128.f)
            , alpha(1.0f)
            , layer(0)
        {
        }*/

        UIScreenInstance(const std::string& _tag,
            const std::string& _texturePath,
            const ImVec2& _pos,
            const ImVec2& _size,
            _float _alpha,
            _int _layer,
            ID3D11ShaderResourceView* _pSRV,
            ImTextureID _imguiTexture)
            : tag(_tag), texturePath(_texturePath), position(_pos), size(_size),
            alpha(_alpha), layer(_layer), pSRV(_pSRV), imguiTexture(_imguiTexture)
        {
        }
    };



    DECLARE_SINGLETON(CImGui_Manager)
private:
	CImGui_Manager();
	~CImGui_Manager() override = default;

private:
    CGameInstance*  m_pGameInstance = { nullptr };

    map<std::string, std::vector<UIElementData>> m_UIFolderMap;
    char    m_szFolderPath[260] = "Textures\\UI";
    vector<UIScreenInstance>    m_PlacedInstances;

    ID3D11Device*           m_pDevice = nullptr;
    ID3D11DeviceContext*    m_pContext = nullptr;

    _float  m_fUIScale = 1.0f;

private:
    void    Show_Multi_UIEditor();
     void   Load_Textures_From_Folder(const std::string& folder);
    _bool   Load_Texture_From_File(const std::string& path, ID3D11ShaderResourceView** outSRV);

public:
	HRESULT Initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context);
	void	Bind();
	void	Render();
	void	Release_ImGui();
	void	Free() override;
};

END