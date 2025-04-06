#include "Base.h"
#include "Input_Manager.h"

CInput_Manager::CInput_Manager(HWND _hWnd) : m_hWnd(_hWnd)
{
	ZeroMemory(m_bKeyState, KEY_IDLE, sizeof(m_bKeyState));
}

bool CInput_Manager::Get_Key(int _iKey) const
{
	if (m_bKeyState[_iKey] & (KEY_PRESSING | KEY_DOWN))
		return true;

	return false;
}

bool CInput_Manager::Key_Pressing(int _iKey) const
{
	if (m_bKeyState[_iKey] == KEY_PRESSING)
		return true;

	return false;
}

bool CInput_Manager::Key_Down(int _iKey) const
{
	if (m_bKeyState[_iKey] == KEY_DOWN)
		return true;

	return false;
}

bool CInput_Manager::Key_Up(int _iKey) const
{
	if (m_bKeyState[_iKey] == KEY_UP)
		return true;

	return false;
}

void CInput_Manager::Update_Key()
{
	for (int i = 0; i < VK_MAX; ++i)
	{
		if (m_bKeyState[i] & (KEY_PRESSING | KEY_DOWN)) //이전에 눌린 적이 있을 때
		{
			if (GetAsyncKeyState(i) & 0x8000) //이번에 눌렸을 때
				m_bKeyState[i] = KEY_PRESSING;

			else //이번에 안 눌렸을 때
				m_bKeyState[i] = KEY_UP;
		}
		else //이전에 눌린 적 없을 때
		{
			if (GetAsyncKeyState(i) & 0x8000) //이번에 눌렸을 때
				m_bKeyState[i] = KEY_DOWN;

			else //이번에 안 눌렸을 때
				m_bKeyState[i] = KEY_IDLE;
		}
	}

	POINT mouse;
	GetCursorPos(&mouse);
	ScreenToClient(m_hWnd, &mouse);
	m_MousePos = _float3{ static_cast<_float>(mouse.x), static_cast<_float>(mouse.y), 0.f };

}


CInput_Manager* CInput_Manager::Create(HWND _hWnd)
{
	return new CInput_Manager(_hWnd);
}

void CInput_Manager::Free()
{
	__super::Free();
}
