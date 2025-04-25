#pragma once

#include <Windows.h>
#include <process.h>

namespace Client
{
	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;

}

#include "Client_Enum.h"
#include "MonsterState.h"
#include "EventTypes.h"

#define  STORESLOTSIZE 15
#define  GEARSLOTSIZE 3
#define  ARTIFACTSLOTSIZE 3

extern		HINSTANCE	g_hInstance;
extern		HWND		g_hWnd;

using namespace Client;