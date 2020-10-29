#include <chrono>
#include <thread>
#include "audiocom.hpp"
#include "spotify.hpp"
#include "UI.hpp"
#include "utility.hpp"

int main()
{
	Spotify spotify;

	// Main loop flags
	bool isRunning = true;           
	bool isFirstIteration = true;

	// Initialize COM library
	if (FAILED(InitializeCOM()))
		ErrorExit("Failed to initialize COM");

	// Starting point for clock
	auto timeStart = std::chrono::steady_clock::now();

	UI ui(spotify, isRunning, timeStart);

	// Main loop
	while (isRunning)
	{	
		// Only enumerate windows every x seconds
		auto timeNow = std::chrono::steady_clock::now();
		if ((timeNow - timeStart) >= std::chrono::seconds(ui.options.interval) || isFirstIteration)
		{
			spotify.UpdateSpotifyProcessInfo();
			spotify.UpdateState();
			
			// Need to clear some data associated with Spotify process
			spotify.Clear();

			timeStart = timeNow;

			if (isFirstIteration)
				isFirstIteration = false;
		}

		ui.SetTime(timeNow);
		ui.PrintUI();
		ui.HandleGlobalInput();
		ui.HandleInput();

		std::this_thread::sleep_for(std::chrono::milliseconds(80));
	}

	CoUninitialize();
	return 0;
}