#include "Renderer.h"
#include "GameObject.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pContext { pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Initialize()
{
    return S_OK;
}

HRESULT CRenderer::Add_RenderObject(RENDERGROUP eRenderGroup, CGameObject* pRenderObject)
{
    if (nullptr == pRenderObject || 
        eRenderGroup >= RENDER_END)
        return E_FAIL;

    m_RenderObjects[eRenderGroup].push_back(pRenderObject);

    Safe_AddRef(pRenderObject);

    return S_OK;
}

HRESULT CRenderer::Draw()
{
    if (FAILED(Render_Priority()))
        return E_FAIL;
    if (FAILED(Render_NonBlend()))
        return E_FAIL;
    if (FAILED(Render_Blend()))
        return E_FAIL;
    if (FAILED(Render_UI()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
    for (auto& pRenderObject : m_RenderObjects[RENDER_PRIORITY])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[RENDER_PRIORITY].clear();

    return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
    for (auto& pRenderObject : m_RenderObjects[RENDER_NONBLEND])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[RENDER_NONBLEND].clear();

    return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
    for (auto& pRenderObject : m_RenderObjects[RENDER_BLEND])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[RENDER_BLEND].clear();

    return S_OK;
}

HRESULT CRenderer::Render_UI()
{
    for (auto& pRenderObject : m_RenderObjects[RENDER_UI])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[RENDER_UI].clear();

    return S_OK;
}

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRenderer* pGameInstance = new CRenderer(pDevice, pContext);

    if (FAILED(pGameInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CRenderer");
        Safe_Release(pGameInstance);
    }

    return pGameInstance;
}


void CRenderer::Free()
{
    __super::Free();

    for (size_t i = 0; i < RENDER_END; i++)
    {
        for (auto& pRenderObject : m_RenderObjects[i])
            Safe_Release(pRenderObject);

        m_RenderObjects[i].clear();
    }

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
