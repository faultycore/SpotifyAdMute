#pragma once

#include <Windows.h>
#include "audiocom.hpp"
#include "utility.hpp"

class UI
{
public:
	UI(EnumData* spotify, bool* flMainLoopFlag);
	~UI();

	VOID PrintUI();
	VOID HandleInput();
	VOID HandleGlobalInput();

private:
	HANDLE hConin, hConout;            // handle to console input buffer
	DWORD dwInitialConsoleModeIn;      // saves initial console input mode
	DWORD dwInitialConsoleModeOut;     // saves initial console output mode
	UINT  uiInitialConsoleCodePage;    // saves initial console code page
	EnumData* spotify;                  // structure that contains some data regarding Spotify process

	bool* flMainLoopFlag;
	bool flKeyDown;
};