#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <Windows.h>
#include "audiocom.h"
#include "wincallback.h"
#include "utility.h"

int main()
{
	HANDLE hConin, hConout;            // handle to console input buffer
	INPUT_RECORD irInputBuffer[128];   // buffer that receives the console input
	DWORD dwInitialConsoleModeIn;      // saves initial console input mode
	DWORD dwInitialConsoleModeOut;     // saves initial console output mode
	DWORD dwConsoleModeIn;             // current console input mode
	DWORD dwConsoleModeOut;            // current console output mode
	UINT  uiInitialConsoleCodePage;    // saves initial console code page
	DWORD cConsoleInput;               // amount of unread inputs in the input buffer
	DWORD cNumRead;                    // amount of inputs read                
	EnumData spotify;                  // structure that saves some data related to spotify process/window

	bool flIsRunning = true;           // main loop flags
	bool flFirstIteration = true;

	// initialize COM library
	if (FAILED(InitializeCOM()))
		ErrorExit("Failed to initialize COM");

	// get console input/output handle
	hConin = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	hConout = CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

	// save current code page of console
	uiInitialConsoleCodePage = GetConsoleCP();

	// (hopefully) enable UTF-8 encoding (probably useless since most consoles won't have needed fonts anyway)
	SetConsoleCP(65001);

	// save current mode of console 
	GetConsoleMode(hConin, &dwInitialConsoleModeIn);
	GetConsoleMode(hConout, &dwInitialConsoleModeOut);

	// disable quick edit mode
	dwConsoleModeIn = ENABLE_EXTENDED_FLAGS | (dwInitialConsoleModeIn & ~ENABLE_QUICK_EDIT_MODE);
	SetConsoleMode(hConin, dwConsoleModeIn);

	// enable processing of VT-100 escape sequences
	dwConsoleModeOut = dwInitialConsoleModeOut | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hConout, dwConsoleModeOut);

	std::cout << "Press [esc] to exit" << std::endl;
	std::cout << "-------------------" << std::endl << std::endl;

	// starting point for clock
	std::chrono::steady_clock::time_point tStart = std::chrono::steady_clock::now();

	// main loop
	while (flIsRunning)
	{
		// only enumerate windows every x seconds
		std::chrono::steady_clock::time_point tNow = std::chrono::steady_clock::now();
		if ((tNow - tStart) >= std::chrono::seconds(2) || flFirstIteration)
		{
			EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&spotify));

			if (!spotify.title.empty())  // check if title is not empty
			{
				for (auto i = spotify.title.begin(); i != spotify.title.end(); ++i)  // loop through titles, need to find the "main" title
				{
					if (*i != L"GDI+ Window (Spotify.exe)" && *i != L"MSCTFIME UI" && *i != L"Default IME" && *i != L"CSpNotify Notify Window")  // these can all be disregarded
					{
						if (*i == L"Spotify Free")  // "Spotify Free" means that nothing is playing
							std::wcout << "\033[1APlaying: None" << "\033[K";
						else if (*i == L"Advertisement" || *i == L"Spotify")  // "Advertisement" or "Spotify" mean that an advertisement is playing
						{
							std::wcout << "\033[1APlaying: Advertisement :(" << "\033[K";
							for (int c = 0; c < 5; c++)  // c is the number of elements in dwProcessID array of EnumData struct, which is number of Spotify processes
							{                            // loop through all processes associated with Spotify, get their audio manager COM object and their mute audio
								IAudioSessionManager2* pSessionManager = NULL;         // pointer to IAudioSessionManager2 COM interface
								ISimpleAudioVolume* pAudioVolume = NULL;               // pointer to ISimpleAudioVolume COM interface
								if (FAILED(GetAudioSessionManager(&pSessionManager)))
								{
									SafeRelease(&pSessionManager);
									SafeRelease(&pAudioVolume);
									ErrorExit("Create Audio Session Manager");
								}
								if (FAILED(EnumAudioSessions(pSessionManager, &pAudioVolume, spotify.dwProcessID[c])))
								{
									SafeRelease(&pSessionManager);
									SafeRelease(&pAudioVolume);
									ErrorExit("Retrieve audio sessions");
								}
								if (pAudioVolume)
									pAudioVolume->SetMute(true, &GUID_NULL);

								SafeRelease(&pSessionManager);
								SafeRelease(&pAudioVolume);
							}
						}
						else if (!CheckForUnicodeCharacters(*i))  // check if song title does not unicode characters (cannot be displayed by most consoles)
						{
							std::wcout << "\033[1APlaying: " << *i << "\033[K";
							for (int c = 0; c < 5; c++)  // same as for advertisement, just unmute instead of mute
							{
								IAudioSessionManager2* pSessionManager = NULL;         // pointer to IAudioSessionManager2 COM interface
								ISimpleAudioVolume* pAudioVolume = NULL;               // pointer to ISimpleAudioVolume COM interface
								if (FAILED(GetAudioSessionManager(&pSessionManager)))
								{
									SafeRelease(&pSessionManager);
									SafeRelease(&pAudioVolume);
									ErrorExit("Create Audio Session Manager");
								}
								if (FAILED(EnumAudioSessions(pSessionManager, &pAudioVolume, spotify.dwProcessID[c])))
								{
									SafeRelease(&pSessionManager);
									SafeRelease(&pAudioVolume);
									ErrorExit("Retrieve audio sessions");
								}
								if (pAudioVolume)
									pAudioVolume->SetMute(false, &GUID_NULL);

								SafeRelease(&pSessionManager);
								SafeRelease(&pAudioVolume);
							}
						}
						else  // song title contains unicode characters
						{
							std::wcout << "\033[1APlaying: Unable to display song name" << "\033[K";
							for (int c = 0; c < 5; c++)  // same as for advertisement, just unmute instead of mute
							{
								IAudioSessionManager2* pSessionManager = NULL;         // pointer to IAudioSessionManager2 COM interface
								ISimpleAudioVolume* pAudioVolume = NULL;               // pointer to ISimpleAudioVolume COM interface
								if (FAILED(GetAudioSessionManager(&pSessionManager)))
								{
									SafeRelease(&pSessionManager);
									SafeRelease(&pAudioVolume);
									ErrorExit("Create Audio Session Manager");
								}
								if (FAILED(EnumAudioSessions(pSessionManager, &pAudioVolume, spotify.dwProcessID[c])))
								{
									SafeRelease(&pSessionManager);
									SafeRelease(&pAudioVolume);
									ErrorExit("Retrieve audio sessions");
								}
								if (pAudioVolume)
									pAudioVolume->SetMute(false, &GUID_NULL);

								SafeRelease(&pSessionManager);
								SafeRelease(&pAudioVolume);
							}
						}
					}
				}
			}
			else  // if title is empty, Spotify process couldn't be found
				std::wcout << "\033[1AUnable to locate Spotify process..." << "\033[K";

			std::wcout << std::endl;

			spotify.title.clear();

			tStart = tNow;

			if (flFirstIteration)
				flFirstIteration = false;
		}

		// need to check if there are entries in the buffer 
		// otherwise ReadConsoleInput blocks the execution of loop until there is
		GetNumberOfConsoleInputEvents(hConin, &cConsoleInput);
		if (cConsoleInput != 0)
		{
			// read input
			ReadConsoleInput(hConin, irInputBuffer, 128, &cNumRead);
			
			// process input
			for (DWORD i = 0; i < cNumRead; ++i)
			{
				WORD ctrlk = 0;
				WORD k = KeyEventProc(irInputBuffer[i].Event.KeyEvent, &ctrlk);
				switch (irInputBuffer[i].EventType)
				{
					// catch key presses
				case KEY_EVENT:
					switch (k)
					{
					case VK_ESCAPE:
							flIsRunning = false;
							SetConsoleMode(hConin, dwInitialConsoleModeIn);
							SetConsoleMode(hConout, dwInitialConsoleModeOut);
							std::wcout << std::endl << "exiting...";
							break;
					case 0x43:
			                std::wcout << std::endl << "qwert";
							break;
					case VK_RIGHT:
						if (ctrlk & 0x0008)
						{
							INPUT inRight;
							inRight.type = INPUT_KEYBOARD;
							inRight.ki.dwExtraInfo = 0;

							//inRight.ki.dwFlags = KEYEVENTF_SCANCODE;
							inRight.ki.dwFlags = 0;

							inRight.ki.time = 0;

							//inRight.ki.wScan = 0x4f;
							inRight.ki.wScan = 0;

							inRight.ki.wVk = VK_RIGHT;

							INPUT inCtrl;
							inCtrl.type = INPUT_KEYBOARD;
							inCtrl.ki.dwExtraInfo = 0;

							//inCtrl.ki.dwFlags = KEYEVENTF_SCANCODE;
							inCtrl.ki.dwFlags = 0;

							inCtrl.ki.time = 0;

							//inCtrl.ki.wScan = 0xe0;
							inCtrl.ki.wScan = 0;

							inCtrl.ki.wVk = VK_LCONTROL;

							INPUT inputs[2] = { inRight, inCtrl };
							//if (SetForegroundWindow(spotify.hWnd))
								//std::wcout << std::endl << "Success";
							//SetFocus(spotify.hWnd);

							PostMessage(spotify.hWnd, WM_KEYDOWN, MapVirtualKey(VK_LCONTROL, MAPVK_VK_TO_VSC), MapVirtualKey(VK_LCONTROL, MAPVK_VK_TO_VSC));
							PostMessage(spotify.hWnd, WM_KEYDOWN, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC), MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC));

							HWND hwndForeground = GetForegroundWindow();
							int length = GetWindowTextLength(hwndForeground);
							wchar_t* buffer = new wchar_t[length];
							GetWindowText(hwndForeground, buffer, length);
							std::wcout << std::endl << std::endl << buffer;
							delete[] buffer;
							
							SendInput(2, inputs, sizeof(INPUT));
							std::wcout << std::endl << "asdf";
						}
							break;
					}
					break;

					// disregard all other events
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

	// return console to its initial state
	SetConsoleCP(uiInitialConsoleCodePage);
	SetConsoleMode(hConin, dwInitialConsoleModeIn);
	SetConsoleMode(hConout, dwInitialConsoleModeOut);
	return 0;
}