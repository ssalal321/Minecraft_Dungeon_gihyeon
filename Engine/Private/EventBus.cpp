#include "EventBus.h"
#include "GameInstance.h"

using namespace DirectX;

CEventBus::CEventBus()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CEventBus::Initialize()
{

	return S_OK;
}


CEventBus* CEventBus::Create()
{
    CEventBus* pInstance = new CEventBus();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CEventBus");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEventBus::Free()
{
    __super::Free();

    // 등록된 모든 핸들러 리스트 해제
    for (auto it = m_Handlers.begin(); it != m_Handlers.end(); ++it)
    {
        IHandlerList* pList = it->second;
        if (pList)
            Safe_Delete(pList);
    }
    m_Handlers.clear(); // map 비우기

    Safe_Release(m_pGameInstance);
}