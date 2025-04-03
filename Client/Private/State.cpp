#include "State.h"



void CState::Free()
{
    Safe_Release(m_pGameInstance);
}
