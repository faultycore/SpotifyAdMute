#include "UI.hpp"

UI::UI(EnumData* spotify, bool* flMainLoopFlag)
{
	// Get console input/output handle
	hConin = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	hConout = CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

	// Save current code page of console
	uiInitialConsoleCodePage = GetConsoleCP();

	// (hopefully) Enable UTF-8 encoding (probably useless since most consoles won't have needed fonts anyway)
	SetConsoleCP(65001);

	// Save current mode of console 
	GetConsoleMode(hConin, &dwInitialConsoleModeIn);
	GetConsoleMode(hConout, &dwInitialConsoleModeOut);

	// Disable quick edit mode
	DWORD dwConsoleModeIn = ENABLE_EXTENDED_FLAGS | (dwInitialConsoleModeIn & ~ENABLE_QUICK_EDIT_MODE);
	SetConsoleMode(hConin, dwConsoleModeIn);

	// Enable processing of VT-100 escape sequences
	DWORD dwConsoleModeOut = dwInitialConsoleModeOut | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hConout, dwConsoleModeOut);

	this->spotify = spotify;
	this->flMainLoopFlag = flMainLoopFlag;
	flKeyDown = false;
}

UI::~UI()
{
	// Return console to its initial state
	SetConsoleCP(uiInitialConsoleCodePage);
	SetConsoleMode(hConin, dwInitialConsoleModeIn);
	SetConsoleMode(hConout, dwInitialConsoleModeOut);
}

VOID UI::HandleInput()
{
	// I don't handle this input with GetAsynchKeyState, because it should
	// only work when the console window is in focus.
	// Need to check if there are entries in the buffer 
	// otherwise ReadConsoleInput blocks the execution of loop until there is

	INPUT_RECORD irInputBuffer[128];  // Buffer that receives console input
	DWORD cConsoleInput;              // Number of console inputs
	DWORD cNumRead;                   // Amount of inputs read 

	GetNumberOfConsoleInputEvents(hConin, &cConsoleInput);
	if (cConsoleInput != 0)
	{
		// Read input
		ReadConsoleInput(hConin, irInputBuffer, 128, &cNumRead);

		// Process input
		for (DWORD i = 0; i < cNumRead; ++i)
		{
			WORD ctrlk = 0;
			WORD k = GetKeyEvent(irInputBuffer[i].Event.KeyEvent, &ctrlk);
			switch (irInputBuffer[i].EventType)
			{
			case KEY_EVENT:  // Catch key presses
				switch (k)
				{
				case VK_ESCAPE:
					*flMainLoopFlag = false;
					std::wcout << std::endl << "exiting...";
					break;
				case 'o':  // Implement: options
						//std::wcout << std::endl << "options";
					break;
				}
				// Disregard all other events
			case MOUSE_EVENT:
			case WINDOW_BUFFER_SIZE_EVENT:
			case FOCUS_EVENT:
			case MENU_EVENT:
				break;

			default:
				ErrorExit("Unknown event type");
				break;
			}
		}
	}
}

VOID UI::HandleGlobalInput()
{
	// Input that should be handled even if console window is not the focus

	bool ctrlRightPressed = GetAsyncKeyState(VK_LCONTROL) & GetAsyncKeyState(VK_RIGHT) & 0x8000;
	bool ctrlLeftPressed = GetAsyncKeyState(VK_LCONTROL) & GetAsyncKeyState(VK_LEFT) & 0x8000;
	bool ctrlSpacePressed = GetAsyncKeyState(VK_LCONTROL) & GetAsyncKeyState(VK_SPACE) & 0x8000;

	INPUT input;
	input.type = INPUT_KEYBOARD;
	input.ki.dwExtraInfo = 0;
	input.ki.dwFlags = 0;
	input.ki.time = 0;
	input.ki.wScan = 0;

	if (ctrlRightPressed && flKeyDown == false)
	{
		input.ki.wVk = VK_MEDIA_NEXT_TRACK;
		SendInput(1, &input, sizeof(INPUT));
		flKeyDown = true;
	}
	else if (ctrlLeftPressed && flKeyDown == false)
	{
		input.ki.wVk = VK_MEDIA_PREV_TRACK;
		SendInput(1, &input, sizeof(INPUT));
		flKeyDown = true;
	}
	else if (ctrlSpacePressed && flKeyDown == false)
	{
		input.ki.wVk = VK_MEDIA_PLAY_PAUSE;
		SendInput(1, &input, sizeof(INPUT));
		flKeyDown = true;
	}
	else if (!ctrlRightPressed && !ctrlLeftPressed && !ctrlSpacePressed)
		flKeyDown = false;
}

VOID UI::PrintUI()
{

}