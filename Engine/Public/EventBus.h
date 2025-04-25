#pragma once

#include "Base.h"
#include <typeindex>

BEGIN(Engine)
class CGameInstance;

class CEventBus final : public CBase
{
private:
    CEventBus();
    ~CEventBus() override = default;

public:
    HRESULT Initialize();


    // handler : 이벤트가 발생했을 때, 그 이벤트를 처리하는 함수
    // 이벤트를 처리하는 로직을 담고 있다.
    template<typename T>
	void   Subscribe(function<void(const T&)> handler)
	{
        type_index typeIndex(typeid(T)); // 타입 정보 저장용 key

        // 핸들러 리스트가 없다면 새로 생성 후 참조 추가
        if (m_Handlers.find(typeIndex) == m_Handlers.end()) 
        {
            auto* newList = new HandlerList<T>();
            m_Handlers[typeIndex] = newList;
        }

        // 해당 타입의 핸들러 리스트에 핸들러 등록
        static_cast<HandlerList<T>*>(m_Handlers[typeIndex])->handlers.push_back(handler);
    }


    // 이벤트 발생 시 등록된 핸들러 호출
    // T: 이벤트 타입   /   event: 실제 발생한 이벤트 데이터
    template<typename T>
    void   Publish(const T& event) const
	{
        type_index typeIndex(typeid(T));

        auto it = m_Handlers.find(typeIndex);

        if (it != m_Handlers.end()) 
        {
            auto* list = static_cast<HandlerList<T>*>(it->second);

            for (const auto& handler : list->handlers) 
            {
                handler(event); // 핸들러 호출
            }
        }
    }

private:
    CGameInstance*  m_pGameInstance = { nullptr };

    struct IHandlerList { virtual ~IHandlerList() = default; };

    // 이벤트 타입별 핸들러 리스트 정의 (템플릿)
    template<typename T>
    struct HandlerList : IHandlerList
	{
        vector<function<void(const T&)>> handlers; // 콜백 리스트
    };

    // 타입별 핸들러 리스트 저장용 맵
    unordered_map<type_index, IHandlerList*>  m_Handlers;

public:
    static  CEventBus*   Create();
    void    Free()  override;
};

END
