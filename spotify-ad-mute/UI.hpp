#pragma once

#include <Windows.h>
#include <chrono>
#include "spotify.hpp"

class UI
{
public:
	UI(const Spotify& spotify, bool& mainLoopFlag, std::chrono::steady_clock::time_point timeStart);
	~UI();

	static const unsigned short MAIN_SCREEN = 0;
	static const unsigned short OPTIONS_SCREEN = 1;
	static const unsigned short OPTION_INTERVAL = 2;
	static const unsigned short OPTION_ASCIIJAM = 3;
	static const unsigned short OPTION_ADDNAME = 4;

	struct Options
	{
		unsigned short interval = 2;
		bool asciiJam = false;
	} options;

	void PrintUI();
	// Input that should be handled only if the console window is the focus
	void HandleInput();
	// Input that should be handled even if console window is not the focus
	void HandleGlobalInput();

	void SetTime(std::chrono::steady_clock::time_point timeNow) { this->timeNow = timeNow; }

private:
	HANDLE hConin, hConout;							// handle to console input buffer
	DWORD consoleInitialModeIn;						// saves initial console input mode
	DWORD consoleInitialModeOut;					// saves initial console output mode
	UINT  consoleInitialCodePage;					// saves initial console code page
	CONSOLE_CURSOR_INFO cursorInfo;					// contains console cursor info
	std::chrono::steady_clock::time_point timeNow;  // clock for animation
	float animationCycle;							// sine of tNow

	const Spotify& spotify;							// Class that contains some data regarding Spotify process
	bool& mainLoopFlag;
	bool isKeyDown;
	int currentProgramState;

	void ClearScreen();
	void PrintMainScreen();
	void PrintOptionsScreen();
	void PrintOptionInterval();
	void PrintOptionAsciiJam();
	void PrintOptionAddName();
	void PrintAsciiJam();
};