#pragma once

#include <Windows.h>
#include <process.h>
#include <vector>

#include "fmod.h"
#include "fmod.hpp"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#pragma comment(lib, "fmod_vc.lib")

extern bool	bMouseClickLock;  // 전역 변수 선언

namespace Client
{
	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;
}

#include "Client_Enum.h"
#include "MonsterState.h"
#include "EventTypes.h"

#define  STORESLOTSIZE 12
#define  GEARSLOTSIZE 3
#define  ARTIFACTSLOTSIZE 3

extern		HINSTANCE	g_hInstance;
extern		HWND		g_hWnd;

using namespace Client;