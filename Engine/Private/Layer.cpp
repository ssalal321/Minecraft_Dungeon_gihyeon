#include "Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Add_GameObject(const _wstring& strGameObjectTag, CGameObject* pGameObject)
{
    if (nullptr == pGameObject)
        return E_FAIL;

    m_GameObjects.insert({ strGameObjectTag, pGameObject });

    return S_OK;
}

CComponent* CLayer::Get_Component(const _wstring& strComponentTag, _uint iIndex)
{
    auto    iter = m_GameObjects.begin();

    for (size_t i = 0; i < iIndex; i++)
        ++iter;

    return iter->second->Find_Component(strComponentTag);
}

void CLayer::Priority_Update(_float fTimeDelta)
{
    for (auto& pGameObject : m_GameObjects)
    {
        if (nullptr != pGameObject.second)
            pGameObject.second->Priority_Update(fTimeDelta);
    }
}

void CLayer::Update(_float fTimeDelta)
{
    for (auto& pGameObject : m_GameObjects)
    {
        if (nullptr != pGameObject.second)
            pGameObject.second->Update(fTimeDelta);
    }
}

void CLayer::Late_Update(_float fTimeDelta)
{
    for (auto& pGameObject : m_GameObjects)
    {
        if (nullptr != pGameObject.second)
            pGameObject.second->Late_Update(fTimeDelta);
    }
}

CGameObject* CLayer::Find_GameObject(const _wstring& strGameObjectTag)
{
    auto iter = m_GameObjects.find(strGameObjectTag);
    if (iter != m_GameObjects.end())
    	return iter->second;

    return nullptr;
}

CLayer* CLayer::Create()
{
    return new CLayer();
}

void CLayer::Free()
{
    __super::Free();

    for (auto& pGameObject : m_GameObjects)
        Safe_Release(pGameObject.second);

    m_GameObjects.clear();
}
