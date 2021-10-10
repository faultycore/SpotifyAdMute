#include <iostream>
#include "UserInterface.hpp"
#include "Spotify.hpp"
#include "Utility.hpp"
#include "Event.hpp"
#include "MainScreen.hpp"
#include "OptionScreen.hpp"
#include "ExitScreen.h"

UserInterface::UserInterface(Data* data) 
{
	setup_console();

	_data = data;
	_isKeyDown = false;
	_state = MAIN;

	Screen* mainScreen = new MainScreen(data);
	_screens.insert(std::pair<int, Screen*>((int)MAIN, mainScreen));

	Screen* optionScreen = new OptionScreen(data);
	_screens.insert(std::pair<int, Screen*>((int)OPTION, optionScreen));

	Screen* exitScreen = new ExitScreen(data);
	_screens.insert(std::pair<int, Screen*>((int)EXIT, exitScreen));

	print_interface(true);
}

UserInterface::~UserInterface()
{
	//Return console to its initial state
	SetConsoleCP(_consoleInitialCodePage);
	SetConsoleMode(_hConin, _consoleInitialModeIn);
	SetConsoleMode(_hConout, _consoleInitialModeOut);

	for (auto e : _screens)
	{
		if (e.second != nullptr)
			delete e.second;
	}

	_screens.clear();
}

void UserInterface::setup_console() 
{
	//Get console input/output handle
	_hConin = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	_hConout = CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

	CONSOLE_SCREEN_BUFFER_INFO scrBufferInfo;
	GetConsoleScreenBufferInfo(_hConout, &scrBufferInfo);

	// Current window size
	short winWidth = scrBufferInfo.srWindow.Right - scrBufferInfo.srWindow.Left + 1;
	short winHeight = scrBufferInfo.srWindow.Bottom - scrBufferInfo.srWindow.Top + 1;

	// Current screen buffer size
	short scrBufferWidth = scrBufferInfo.dwSize.X;
	short scrBufferHeight = scrBufferInfo.dwSize.Y;

	// To remove the scrollbar, make sure the window height matches the screen buffer height
	COORD newSize;
	newSize.X = scrBufferWidth;
	newSize.Y = winHeight;

	// Set the new screen buffer dimensions
	SetConsoleScreenBufferSize(_hConout, newSize);

	// Save current code page of console
	_consoleInitialCodePage = GetConsoleCP();

	// (hopefully) Enable UTF-8 encoding (probably useless since most consoles won't have needed fonts anyway)
	SetConsoleCP(65001);

	// Save current mode of console 
	GetConsoleMode(_hConin, &_consoleInitialModeIn);
	GetConsoleMode(_hConout, &_consoleInitialModeOut);

	// Hide console cursor
	 GetConsoleCursorInfo(_hConout, &_cursorInfo);
	 _cursorInfo.bVisible = false;
	 SetConsoleCursorInfo(_hConout, &_cursorInfo);

	//Disable quick edit mode
	DWORD consoleModeIn = ENABLE_EXTENDED_FLAGS | (_consoleInitialModeIn & ~ENABLE_QUICK_EDIT_MODE);
	SetConsoleMode(_hConin, consoleModeIn);
}

void UserInterface::print_interface(bool forcePrint) const
{
	if (forcePrint)
		_screens.at(_state)->force_reprint();
	else
		_screens.at(_state)->print();
}


void UserInterface::handle_input() 
{
	// Handle "global" hotkeys
	bool isCtrlPressed	= GetAsyncKeyState(VK_LCONTROL) & 0x8000;
	bool isShiftPressed = GetAsyncKeyState(VK_LSHIFT) & 0x8000;
	bool isRightPressed = GetAsyncKeyState(VK_RIGHT) & 0x8000;
	bool isLeftPressed	= GetAsyncKeyState(VK_LEFT) & 0x8000;
	bool isSpacePressed = GetAsyncKeyState(VK_SPACE) & 0x8000;

	INPUT input;
	input.type = INPUT_KEYBOARD;
	input.ki.dwExtraInfo = 0;
	input.ki.dwFlags = 0;
	input.ki.time = 0;
	input.ki.wScan = 0;

	if (isCtrlPressed && isRightPressed && !_isKeyDown)
	{
		_isKeyDown = true;
		input.ki.wVk = VK_MEDIA_NEXT_TRACK;
		SendInput(1, &input, sizeof(INPUT));
	}
	else if (isCtrlPressed && isLeftPressed && !_isKeyDown)
	{
		_isKeyDown = true;
		input.ki.wVk = VK_MEDIA_PREV_TRACK;
		SendInput(1, &input, sizeof(INPUT));
	}
	else if (isCtrlPressed && isSpacePressed && !_isKeyDown)
	{
		_isKeyDown = true;
		input.ki.wVk = VK_MEDIA_PLAY_PAUSE;
		SendInput(1, &input, sizeof(INPUT));
	}

	if (!(isRightPressed || isLeftPressed || isSpacePressed || isCtrlPressed))
		_isKeyDown = false;

	// Not handled using GetAsynchKeyState, because it should only work when the console window is in focus
	INPUT_RECORD inputBuffer[128];  // Buffer that receives console input
	DWORD consoleInput;             // Number of console inputs
	DWORD numRead;                  // Amount of inputs read 
	int tmp;

	// Need to check if there are entries in the buffer otherwise ReadConsoleInput blocks 
	GetNumberOfConsoleInputEvents(_hConin, &consoleInput);
	if (consoleInput != 0)
	{
		// Read input
		ReadConsoleInput(_hConin, inputBuffer, 128, &numRead);
		
		// Process input
		for (DWORD i = 0; i < numRead; ++i)
		{
			WORD ctrlk = 0;  // Flag that contains state of control keys
			WORD k = get_key_event(inputBuffer[i].Event.KeyEvent, &ctrlk);
			switch (inputBuffer[i].EventType)
			{
			case KEY_EVENT:  // Catch key presses
				tmp = _screens.at(_state)->handle_input(k);
				if (tmp != NO_CHANGE)
				{
					_state = tmp;
					print_interface(true);
				}
				break;
			case WINDOW_BUFFER_SIZE_EVENT:
				print_interface(true);
				// For some reason the cursor needs to be hidden again after a window resize
				GetConsoleCursorInfo(_hConout, &_cursorInfo);
				_cursorInfo.bVisible = false;
				SetConsoleCursorInfo(_hConout, &_cursorInfo);
				break;
				// Disregard all other events
			default:
				break;
			}
		}
	}
}




