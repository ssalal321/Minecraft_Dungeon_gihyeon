#include "ImGui_Manager.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "UI_Image.h"
#include "GameInstance.h"
#include "DirectXTK/WICTextureLoader.h"
#include "DirectXTK/DDSTextureLoader.h"

IMPLEMENT_SINGLETON(CImGui_Manager)

CImGui_Manager::CImGui_Manager() : m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

#include <windows.h>
#include <fstream>
#include <algorithm>


void CImGui_Manager::Show_Multi_UIEditor()
{
    ImGui::InputText("Folder Path", m_szFolderPath, IM_ARRAYSIZE(m_szFolderPath));
    ImGui::SliderFloat("UI Scale", &m_fUIScale, 0.1f, 5.0f, "%.2fx");

    if (ImGui::Button("Load All Images"))
    {
        Load_Textures_From_Folder(m_szFolderPath);
    }

    ImGui::SeparatorText("Loaded Texture Assets");

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

                if (ImGui::Selectable(ui.texturePath.c_str()))
                {
                    ID3D11Resource* pResource = nullptr;
                    D3D11_TEXTURE2D_DESC desc{};

                    if (ui.pTextureSRV)
                    {
                        ui.pTextureSRV->GetResource(&pResource);
                        if (pResource)
                        {
                            ID3D11Texture2D* pTexture2D = nullptr;
                            if (SUCCEEDED(pResource->QueryInterface(&pTexture2D)))
                            {
                                pTexture2D->GetDesc(&desc);
                                Safe_Release(pTexture2D);
                            }
                            Safe_Release(pResource);
                        }
                    }


                    const float sizeX = static_cast<float>(desc.Width) * m_fUIScale;
                    const float sizeY = static_cast<float>(desc.Height) * m_fUIScale;

                    CUI_Image::UIIMAGE_DESC imageDesc{
                        TEXT("GameObject_UIImage"),
                        CUIObject::UI_STATE::UNCLICKABLE,
                        ui.position.x, ui.position.y, 0.f,
                        sizeX, sizeY,
                        std::wstring(ui.texturePath.begin(), ui.texturePath.end()),
                        LEVEL_STATIC, LEVEL_STATIC,
                        true
                    };
                    // 지금 texPrototypeTag는 파일 경로를 저장 중임
                    // 어차피 imgui에서 직접 불러올 거면 prototypetag 없으므로 imgui 전용 ui_image 만든다고 치고
                    // 파일 경로로 바꾸든가 해야 함
                    // 그리고 TextureCom - Add_Component 실패함(당연함 prototypeTag에 파일 경로 넣어버림)
                    // 이것도 Create TextureCom으로 바꾸든가 해야 할 듯


                    CUI_Image* pUIImage = CUI_Image::Create(m_pDevice, m_pContext);
                    if (pUIImage && SUCCEEDED(pUIImage->Initialize(&imageDesc)))
                    {
                        m_PlacedInstances.push_back({
                            "GameObject_UIImage", ui.texturePath,
                            ui.position, ImVec2(sizeX, sizeY), ui.alpha, 0,
                            ui.pTextureSRV, reinterpret_cast<ImTextureID>(ui.pTextureSRV)
                            });
                    }
                    else
                    {
                        Safe_Release(pUIImage);
                    }
                }

                if (ui.pTextureSRV)
                    ImGui::Image(reinterpret_cast<ImTextureID>(ui.pTextureSRV), ImVec2(64, 64));

                ImGui::Separator();
                ImGui::PopID();
            }

            ImGui::TreePop();
        }
    }

    ImGui::SeparatorText("Placed UI Elements");
    for (size_t i = 0; i < m_PlacedInstances.size(); ++i)
    {
        ImGui::PushID(static_cast<_int>(i));
        UIScreenInstance& inst = m_PlacedInstances[i];
        ImGui::Text("%s", inst.texturePath.c_str());
        ImGui::SliderFloat2("Pos", (_float*)&inst.position, 0.f, 1920.f);
        ImGui::SliderFloat2("Size", (_float*)&inst.size, 0.f, 1080.f);
        ImGui::SliderFloat("Alpha", &inst.alpha, 0.f, 1.f);
        ImGui::SliderInt("Layer", &inst.layer, 0, 10);
        if (inst.imguiTexture)
            ImGui::Image(inst.imguiTexture, inst.size);
        ImGui::Separator();
        ImGui::PopID();
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

    Safe_Release(m_pGameInstance);

    __super::Free();
}