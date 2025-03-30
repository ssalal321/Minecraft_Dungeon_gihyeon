#pragma once

#include <Windows.h>
#include <process.h>

namespace Client
{
	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;

	enum LEVEL { LEVEL_LOADING, LEVEL_STATIC, LEVEL_TITLE, LEVEL_LOUNGE, LEVEL_GAMEPLAY, LEVEL_END };
}

extern		HINSTANCE	g_hInstance;
extern		HWND		g_hWnd;


using namespace Client;
