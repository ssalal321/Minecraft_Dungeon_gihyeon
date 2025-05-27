#include "ImGui_Manager.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "DirectXTK/WICTextureLoader.h"
#include "DirectXTK/DDSTextureLoader.h"

IMPLEMENT_SINGLETON(CImGui_Manager)

#include <windows.h>
#include <fstream>
#include <algorithm>

void CImGui_Manager::Show_Multi_UIEditor()
{
    ImGui::InputText("Folder Path", m_szFolderPath, IM_ARRAYSIZE(m_szFolderPath));
    if (ImGui::Button("Load All Images"))
    {
        Load_Textures_From_Folder(m_szFolderPath);
    }

    ImGui::SeparatorText("Loaded UI Elements");

    for (std::map<std::string, std::vector<UIElementData>>::iterator it = m_UIFolderMap.begin(); it != m_UIFolderMap.end(); ++it)
    {
        const std::string& folder = it->first;
        std::vector<UIElementData>& elements = it->second;

        if (ImGui::TreeNode(folder.c_str()))
        {
            for (int i = 0; i < elements.size(); ++i)
            {
                ImGui::PushID(i);

                UIElementData& ui = elements[i];

                ImGui::Text("%s", ui.texturePath.c_str());

                if (ui.imguiTexture)
                    ImGui::Image(ui.imguiTexture, ImVec2(100, 100));

                ImGui::Separator();
                ImGui::PopID();
            }

            ImGui::TreePop();
        }
    }
}


void CImGui_Manager::Load_Textures_From_Folder(const std::string& folder)
{
    OutputDebugStringA(("[탐색 시작]: " + folder + "\n").c_str());

    DWORD attrs = GetFileAttributesA(folder.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES || !(attrs & FILE_ATTRIBUTE_DIRECTORY))
    {
        OutputDebugStringA("[X] 유효하지 않은 폴더 경로입니다.\n");
        return;
    }

    std::string searchPath = folder + "\\*.*";
    WIN32_FIND_DATAA fd;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &fd);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        OutputDebugStringA("[X] 폴더 내 파일 탐색 실패: FindFirstFile 실패\n");
        return;
    }

    do
    {
        const std::string filename = fd.cFileName;

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (filename != "." && filename != "..")
            {
                std::string subfolder = folder + "\\" + filename;
                Load_Textures_From_Folder(subfolder);
            }
        }
        else
        {
            std::string ext = filename.substr(filename.find_last_of('.') + 1);
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

            if (ext == "png" || ext == "jpg" || ext == "dds")
            {
                std::string fullPath = folder + "\\" + filename;
                OutputDebugStringA(("[이미지 시도] : " + fullPath + "\n").c_str());

                UIElementData element;
                element.texturePath = fullPath;

                if (Load_Texture_From_File(fullPath, &element.pTextureSRV))
                {
                    element.imguiTexture = reinterpret_cast<ImTextureID>(element.pTextureSRV);
                    OutputDebugStringA(("[O] 텍스처 로드 성공: " + fullPath + "\n").c_str());

                    size_t lastSlash = folder.find_last_of("\\/");
                    std::string folderName = (lastSlash == std::string::npos) ? folder : folder.substr(lastSlash + 1);
                    m_UIFolderMap[folderName].push_back(std::move(element));
                }
                else
                {
                    OutputDebugStringA(("[X] 텍스처 로드 실패: " + fullPath + "\n").c_str());
                }
            }
        }

    } while (FindNextFileA(hFind, &fd));

    FindClose(hFind);
}

_bool CImGui_Manager::Load_Texture_From_File(const std::string& path, ID3D11ShaderResourceView** outSRV)
{
    std::wstring wpath(path.begin(), path.end());

    std::string ext = path.substr(path.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    HRESULT hr = E_FAIL;

    if (ext == "dds")
    {
        hr = DirectX::CreateDDSTextureFromFile(m_pDevice, m_pContext, wpath.c_str(), nullptr, outSRV);
    }
    else if (ext == "png" || ext == "jpg")
    {
        hr = DirectX::CreateWICTextureFromFile(m_pDevice, m_pContext, wpath.c_str(), nullptr, outSRV);
    }
    else
    {
        return false;
    }

    if (FAILED(hr) || *outSRV == nullptr)
        return false;

    return true;
}

ID3D11ShaderResourceView* CImGui_Manager::Create_Fallback_Purple_Texture()
{
    static ID3D11ShaderResourceView* s_pFallback = nullptr;

    if (s_pFallback)
        return s_pFallback;

    uint32_t purplePixel = 0xFFFF00FF;

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = 1;
    desc.Height = 1;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.SampleDesc.Count = 1;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = &purplePixel;
    initData.SysMemPitch = sizeof(uint32_t);

    ID3D11Texture2D* pTex = nullptr;
    HRESULT hr = m_pDevice->CreateTexture2D(&desc, &initData, &pTex);
    if (FAILED(hr)) return nullptr;

    hr = m_pDevice->CreateShaderResourceView(pTex, nullptr, &s_pFallback);
    Safe_Release(pTex);

    return s_pFallback;
}

HRESULT CImGui_Manager::Initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context)
{
    m_pDevice = device;
    Safe_AddRef(m_pDevice);
    m_pContext = context;
    Safe_AddRef(m_pContext);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device, context);

    return S_OK;
}

void CImGui_Manager::Bind()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    Show_Multi_UIEditor();

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
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    __super::Free();
}
