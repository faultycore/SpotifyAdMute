#pragma once

#include <Windows.h>
#include <chrono>
#include <list>
#include "Screen.hpp"

typedef std::chrono::steady_clock::time_point Time;

class UserInterface
{
public:
	UserInterface(Data* data);

	~UserInterface();

	void handle_input();

	void print_interface(bool forcePrint) const;

	const int& state() const { return _state; };

private:
	HANDLE _hConin, _hConout;							// Handles to console buffer
	DWORD _consoleInitialModeIn;						// Saves initial console input mode
	DWORD _consoleInitialModeOut;						// Saves initial console output mode
	UINT  _consoleInitialCodePage;						// Saves initial console code page
	CONSOLE_CURSOR_INFO _cursorInfo;					// Contains console cursor info
									
	int _state;
	bool _isKeyDown;

	std::map<int, Screen*> _screens;

	Data* _data;

	void setup_console();
};