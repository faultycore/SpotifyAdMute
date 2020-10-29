#include "UI.hpp"
#include "utility.hpp"
#include <iostream>

UI::UI(const Spotify& spotify, bool& mainLoopFlag, std::chrono::steady_clock::time_point timeStart) : spotify(spotify), mainLoopFlag(mainLoopFlag)
{                      
	// Get console input/output handle
	hConin = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	hConout = CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

	// Save current code page of console
	consoleInitialCodePage = GetConsoleCP();

	// (hopefully) Enable UTF-8 encoding (probably useless since most consoles won't have needed fonts anyway)
	SetConsoleCP(65001);

	// Save current mode of console 
	GetConsoleMode(hConin, &consoleInitialModeIn);
	GetConsoleMode(hConout, &consoleInitialModeOut);

	// Hide console cursor
	GetConsoleCursorInfo(hConout, &cursorInfo);
	cursorInfo.bVisible = false; 
	SetConsoleCursorInfo(hConout, &cursorInfo);

	// Disable quick edit mode
	DWORD consoleModeIn = ENABLE_EXTENDED_FLAGS | (consoleInitialModeIn & ~ENABLE_QUICK_EDIT_MODE);
	SetConsoleMode(hConin, consoleModeIn);

	// Enable processing of VT-100 escape sequences
	DWORD consoleModeOut = consoleInitialModeOut | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hConout, consoleModeOut);

	this->mainLoopFlag = mainLoopFlag;
	this->timeNow = timeStart;
	isKeyDown = false;
	currentProgramState = MAIN_SCREEN; 
	animationCycle = 0;
}

UI::~UI()
{
	// Return console to its initial state
	SetConsoleCP(consoleInitialCodePage);
	SetConsoleMode(hConin, consoleInitialModeIn);
	SetConsoleMode(hConout, consoleInitialModeOut);
}

void UI::HandleInput()
{
	// I don't handle this input with GetAsynchKeyState, because it should
	// only work when the console window is in focus.
	// Need to check if there are entries in the buffer 
	// otherwise ReadConsoleInput blocks the execution of loop until there is

	INPUT_RECORD inputBuffer[128];  // Buffer that receives console input
	DWORD consoleInput;             // Number of console inputs
	DWORD numRead;                  // Amount of inputs read 

	GetNumberOfConsoleInputEvents(hConin, &consoleInput);
	if (consoleInput != 0)
	{
		// Read input
		ReadConsoleInput(hConin, inputBuffer, 128, &numRead);

		// Process input
		for (DWORD i = 0; i < numRead; ++i)
		{
			WORD ctrlk = 0;  // Flag that contains state of control keys
			WORD k = GetKeyEvent(inputBuffer[i].Event.KeyEvent, &ctrlk);
			switch (inputBuffer[i].EventType)
			{
			case KEY_EVENT:  // Catch key presses
				switch (k)
				{
				case VK_ESCAPE:
					mainLoopFlag = false;
					std::wcout << std::endl << "exiting...";
					break;
				case 0x4F:
					if (!(currentProgramState == OPTIONS_SCREEN))
					{
						ClearScreen();
						currentProgramState = OPTIONS_SCREEN;
					}
					else
					{
						ClearScreen();
						currentProgramState = MAIN_SCREEN;
					}
					break;
				case 0x31:
					if (currentProgramState == OPTIONS_SCREEN)
					{
						ClearScreen();
						currentProgramState = OPTION_INTERVAL;
					}
					break;
				case 0x32:
					if (currentProgramState == OPTIONS_SCREEN)
					{
						ClearScreen();
						currentProgramState = OPTION_ASCIIJAM;
					}
					break;
				case 0x33:
					if (currentProgramState == OPTIONS_SCREEN)
					{
						ClearScreen();
						currentProgramState = OPTION_ADDNAME;
					}
				}
			case WINDOW_BUFFER_SIZE_EVENT:
				// For some reason the cursor needs to be hidden again after a window resize
				GetConsoleCursorInfo(hConout, &cursorInfo);
				cursorInfo.bVisible = false;
				SetConsoleCursorInfo(hConout, &cursorInfo);
				break;
			// Disregard all other events
			case MOUSE_EVENT:
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

void UI::HandleGlobalInput()
{
	bool isCtrlPressed = GetAsyncKeyState(VK_LCONTROL) & 0x8000;
	bool isRightPressed = GetAsyncKeyState(VK_RIGHT) & 0x8000;
	bool isLeftPressed = GetAsyncKeyState(VK_LEFT) & 0x8000;
	bool isSpacePressed = GetAsyncKeyState(VK_SPACE) & 0x8000;

	INPUT input;
	input.type = INPUT_KEYBOARD;
	input.ki.dwExtraInfo = 0;
	input.ki.dwFlags = 0;
	input.ki.time = 0;
	input.ki.wScan = 0;

	if (isCtrlPressed && isRightPressed && isKeyDown == false)
	{
		isKeyDown = true;
		input.ki.wVk = VK_MEDIA_NEXT_TRACK;
		SendInput(1, &input, sizeof(INPUT));
	}
	else if (isCtrlPressed && isLeftPressed && isKeyDown == false)
	{
		isKeyDown = true;
		input.ki.wVk = VK_MEDIA_PREV_TRACK;
		SendInput(1, &input, sizeof(INPUT));
	}
	else if (isCtrlPressed && isSpacePressed && isKeyDown == false)
	{
		isKeyDown = true;
		input.ki.wVk = VK_MEDIA_PLAY_PAUSE;
		SendInput(1, &input, sizeof(INPUT));
	}

	if (!isRightPressed && !isLeftPressed && !isSpacePressed && !isCtrlPressed)
		isKeyDown = false;
}

void UI::PrintUI()
{
	std::wcout << "\033[2;0f";
	switch (currentProgramState)
	{
	case MAIN_SCREEN:
		PrintMainScreen();
		break;
	case OPTIONS_SCREEN:
		PrintOptionsScreen();
		break;
	case OPTION_INTERVAL:
		PrintOptionInterval();
		break;
	case OPTION_ASCIIJAM:
		PrintOptionAsciiJam();
		break;
	case OPTION_ADDNAME:
		PrintOptionAddName();
		break;
	default:
		ErrorExit("Unknown UI state");
	}
}

void UI::ClearScreen()
{
	std::wcout << "\033[2;0f";
	for (int i = 0; i < 30; i++)
		std::wcout << "\033[K" << std::endl;
}

void UI::PrintMainScreen()
{
	std::cout << "Press [esc] to exit" << std::endl;
	std::cout << "Press [o] for options/more keybinds" << std::endl;
	std::cout << "-----------------------------------" << std::endl << std::endl;
	switch (spotify.GetState())
	{
	case Spotify::NONE:
		std::wcout << "Playing: None";
		break;
	case Spotify::ADV:
		std::wcout << "Playing: Advertisement :(";
		break;
	case Spotify::SONG:
		std::wcout << "Playing: " << spotify.GetSongTitle();
		break;
	case Spotify::SONGUNI:
		std::wcout << "Unable to display song title";
		break;
	case Spotify::NOTFOUND:
		std::wcout << "Unable to locate Spotify process...";
		break;
	case Spotify::SPOTERROR:
		ErrorExit("Error while checking for Spotify window");
	}
	std::wcout << "\033[K";

	if (options.asciiJam)
		PrintAsciiJam();
}

void UI::PrintOptionsScreen()
{
	std::cout << "Press [esc] to exit" << std::endl;
	std::cout << "Press [o] again to get back" << std::endl;
	std::cout << "---------------------------" << std::endl << std::endl;
	std::cout << "Press the number associated with an option to change it." << std::endl << std::endl;
	std::wcout << "KEYBINDS: " << std::endl;
	std::wcout << "Next track        [ctrl + right]" << std::endl;
	std::wcout << "Previous track    [ctrl + left]" << std::endl;
	std::wcout << "Pause/Unpause     [ctrl + space]" << std::endl;
	std::wcout << std::endl;
	std::wcout << "OPTIONS: " << std::endl;
	std::wcout << "1. Interval       " << options.interval << " second(s)" << std::endl;
	std::wcout << "2. ASCII Jam      " << options.asciiJam << std::endl;
	std::wcout << "3. Add name";
}

void UI::PrintOptionInterval()
{
	std::cout << "Enter new interval in seconds <x>: ";
	cursorInfo.bVisible = true;
	SetConsoleCursorInfo(hConout, &cursorInfo);

	char in;
	std::cin >> in;

	while (!std::isdigit(in))
	{
		ClearScreen();
		std::cout << "\033[2;0fPlease enter a valid integer: ";
		
		std::cin.clear();
		std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
		std::cin >> in;
	}

	options.interval = in - '0';

	ClearScreen();
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(hConout, &cursorInfo);
	currentProgramState = OPTIONS_SCREEN;
}

void UI::PrintOptionAsciiJam()
{
	std::cout << "Enable ASCII Jam? <y/n>: ";
	cursorInfo.bVisible = true;
	SetConsoleCursorInfo(hConout, &cursorInfo);

	char in;
	std::cin >> in;

	while (in != 'y' && in != 'n')
	{
		ClearScreen();
		std::cout << "\033[2;0fPlease enter y or n: ";
		
		std::cin.clear();
		std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
		std::cin >> in;
	}
	
	if (in == 'y')
		options.asciiJam = true;
	else
		options.asciiJam = false;

	ClearScreen();
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(hConout, &cursorInfo);
	currentProgramState = OPTIONS_SCREEN;	
}

void UI::PrintOptionAddName()
{
	std::cout << "This option allows you to manually add Spotify window titles that Spotify should mute when they appear." << std::endl;
	std::cout << "Use this if an ad doesn't get muted automatically. You need to copy the exact title that appears after 'Playing: '" << std::endl << std::endl;
	std::cout << "List of window titles currently getting muted: " << std::endl;

	for (auto i : spotify.GetTitlesAdvert())
		std::wcout << i << std::endl;

	std::cout << std::endl;
	std::cout << "NOT IMPLEMENTED YET! Press enter to get back." << std::endl;
	std::cout << "---------------------------------------------" << std::endl;

	cursorInfo.bVisible = true;
	SetConsoleCursorInfo(hConout, &cursorInfo);
	std::cin.ignore();
	ClearScreen();
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(hConout, &cursorInfo);
	currentProgramState = OPTIONS_SCREEN;
}

void UI::PrintAsciiJam()
{
	auto t = std::chrono::duration<double>(timeNow.time_since_epoch()).count();
	animationCycle = sin(t);
	std::string sprite1 = "                 ,)\033[K\n             ,`/ |\\\033[K\n             /  `/ |\033[K\n           (,_,/  /\033[K\n            `___, \\\033[K\n                 \\ \\\033[K\n                / , \\\033[K\n               /,  \\ \\\033[K\n               \\`   `.`.\033[K\n                \\ \\   \\,`,\033[K\n               -`-'    -'\033[K\n";
	std::string sprite2 = "                    (,\033[K\n               ,`/  /|\033[K\n               /L',/ |\033[K\n           ,)_,,\\', /\033[K\n            `___,V |\033[K\n                 \\ ,\033[K\n                / , \\\033[K\n               / , \\ \\\033[K\n               \\`   `.`.\033[K\n                \\ \\   \\,`,\033[K\n               -`-'   -'\033[K\n";
	std::cout << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;

	if (animationCycle < -0.5)
		std::cout << sprite1;
	else if (animationCycle < 0)
		std::cout << sprite2;
	else if (animationCycle < 0.5)
		std::cout << sprite1;
	else
		std::cout << sprite2;

	std::cout << std::endl << std::endl << "    Credits: ejm @https://asciiart.website/";
}
