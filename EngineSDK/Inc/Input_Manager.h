#pragma once
#include "Base.h"

BEGIN(Engine)
class CInput_Manager : public CBase
{
private:
	CInput_Manager(HWND _hWnd);
	~CInput_Manager() = default;

public:
	bool		Get_Key(int _iKey) const; //현재 프레임에 눌렸는지 여부
	bool		Key_Pressing(int _iKey) const; //계속 눌리는 중일 때
	bool		Key_Down(int _iKey) const; //처음 눌릴 때
	bool		Key_Up(int _iKey) const; //처음 눌리지 않을 때

public:
	_float3		Get_MousePos() const { return m_MousePos; }
	//void		Set_MousePos(const _float3& _mousePos) { m_MousePos = _mousePos; }
	void		Update_Key();
	

private:
	enum KEYSTATE
	{
		KEY_IDLE = 0b0000, //눌리지 않은 상태
		KEY_PRESSING = 0b0001, //눌리고 있는 상태
		KEY_DOWN = 0b0010, // 이전에 눌린 적 없는데 이번에 눌린 상태
		KEY_UP = 0b0100, // 이전에 눌렸는데 이번에 안눌린 상태
	};

	HWND		m_hWnd;
	KEYSTATE	m_bKeyState[VK_MAX];
	_float3		m_MousePos;
	bool		m_bEditor;

public:
	static	CInput_Manager*		Create(HWND _hWnd);
	void						Free()		override;
};
END