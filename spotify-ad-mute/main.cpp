#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <thread>
#include <Windows.h>
#include "audiocom.hpp"
#include "callbacks.hpp"
#include "utility.hpp"
#include "UI.hpp"

int main()
{
	EnumData spotify;                  // Structure that contains some data regarding the Spotify process
	bool flIsRunning = true;           // Main loop flags
	bool flFirstIteration = true;
	bool flShowOptions = false;

	UI ui(&spotify, &flIsRunning);

	std::vector<std::wstring> SpotifyWindowNamesIgnore{ L"GDI+ Window (Spotify.exe)", L"MSCTFIME UI", L"Default IME",  // Window names to ignore
														  L"CSpNotify Notify Window", L"G", L"Drag" };                            
	std::vector<std::wstring> SpotifyWindowNamesAdvert{ L"Advertisement", L"Spotify" };                                // Window names that signify advertisement playing         
	std::vector<std::wstring> SpotifyWindowNamesNone{ L"Spotify Free", L"Spotify Premium" };                           // Window names that signify nothing playing

	// Initialize COM library
	if (FAILED(InitializeCOM()))
		ErrorExit("Failed to initialize COM");

	std::cout << "Press [esc] to exit" << std::endl;
	std::cout << "Press [ctrl + right] or [ctrl + left] for next/previous track" << std::endl;
	std::cout << "-------------------" << std::endl << std::endl;

	// Starting point for clock
	std::chrono::steady_clock::time_point tStart = std::chrono::steady_clock::now();

	// Main loop
	while (flIsRunning)
	{
		if (flShowOptions)
			ShowOptions();

		// Only enumerate windows every x seconds
		std::chrono::steady_clock::time_point tNow = std::chrono::steady_clock::now();
		if ((tNow - tStart) >= std::chrono::seconds(2) || flFirstIteration)
		{
			EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&spotify));

			if (!spotify.title.empty())                                                    // Check if title is not empty
			{
				for (auto i = spotify.title.begin(); i != spotify.title.end(); ++i)        // Loop through titles, need to find the "main" title
				{
					if (!Contains(SpotifyWindowNamesIgnore, *i))                           // Discard irrelevant windows
					{
						if (Contains(SpotifyWindowNamesNone, *i))                          // Check if nothing is playing
							std::wcout << "\033[1APlaying: None" << "\033[K";
						else if (Contains(SpotifyWindowNamesAdvert, *i))                   // Check if an advert is playing
						{
							std::wcout << "\033[1APlaying: Advertisement :(" << "\033[K";
							if (FAILED(ChangeMuteStatus(spotify, true)))
								ErrorExit("Error while trying to mute/unmute Spotify");
						}
						else if (!ContainsUnicodeCharacters(*i))                           // Check if song title does not unicode characters (cannot be displayed by most consoles)
						{
							std::wcout << "\033[1APlaying: " << *i << "\033[K";	
							if (FAILED(ChangeMuteStatus(spotify, false)))
								ErrorExit("Error while trying to mute/unmute Spotify");	
						}
						else                                                               // Song title contains unicode characters
						{
							if (FAILED(ChangeMuteStatus(spotify, false)))
								ErrorExit("Error while trying to mute/unmute Spotify");
						}
					}
				}
			}
			else                                                                           // If title is empty, Spotify process couldn't be found
				std::wcout << "\033[1AUnable to locate Spotify process..." << "\033[K";

			std::wcout << std::endl;

			spotify.title.clear();

			tStart = tNow;

			if (flFirstIteration)
				flFirstIteration = false;
		}

		ui.HandleGlobalInput();
		ui.HandleInput();

		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}

	return 0;
}