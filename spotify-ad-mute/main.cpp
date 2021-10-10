#include <chrono>
#include <thread>
#include "Audiocom.hpp"
#include "Spotify.hpp"
#include "UserInterface.hpp"
#include "Utility.hpp"

int main()
{    
	// Initialize COM interface
	if (FAILED(initialize_com()))
		error_exit("Failed to initialize COM");

	// Starting point for clock
	auto timeStart = std::chrono::steady_clock::now();

	Data data = Data();
	Spotify spotify = Spotify(&data);
	UserInterface ui = UserInterface(&data);
	
	spotify.update_spotify_proc_info();
	spotify.update_state();

	// Main loop
	do
	{	
		// Only enumerate windows every x seconds
		auto timeNow = std::chrono::steady_clock::now();
		if ((timeNow - timeStart) >= std::chrono::seconds(data.options().interval))
		{	
			spotify.procID = 0;
			spotify.title = L"";

			spotify.update_spotify_proc_info();
			spotify.update_state();

			timeStart = timeNow;
		}

		ui.handle_input();
		ui.print_interface(false);

		std::this_thread::sleep_for(std::chrono::milliseconds(80));
	} while (ui.state() != EXIT);

	CoUninitialize();

	return 0;
}