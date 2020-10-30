#include "spotify.hpp"
#include "callbacks.hpp"
#include "audiocom.hpp"
#include "utility.hpp"
#include <combaseapi.h>

Spotify::Spotify()
{
	titlesIgnore.insert(titlesIgnore.end(), { L"GDI+ Window (Spotify.exe)", L"MSCTFIME UI", // Window names to ignore
		                                      L"Default IME", L"CSpNotify Notify Window", L"G", L"Drag" });                             
	titlesAdvert.insert(titlesAdvert.end(), { L"Advertisement", L"Spotify" }); // Window names that signify advertisement playing  			
	titlesNone.insert(titlesNone.end(), { L"Spotify Free", L"Spotify Premium" }); // Window names that signify nothing playing

	state = -1;
}

void Spotify::UpdateSpotifyProcessInfo()
{
	EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(this));
}

void Spotify::UpdateState()
{
	if (!titles.empty()) // Check if title is not empty
	{
		for (auto i = titles.begin(); i != titles.end(); ++i) // Loop through titles, need to find the "main" title
		{
			if (!Contains(titlesIgnore, *i)) // Discard irrelevant windows
			{
				if (Contains(titlesNone, *i)) // Check if nothing is playing
					state = NONE;
				else if (Contains(titlesAdvert, *i)) // Check if an advert is playing
				{
					if (FAILED(ChangeMuteStatus(processIDs, true)))
						ErrorExit("Error while trying to mute/unmute Spotify");
					state = ADV;
				}
				else if (i->find(L"-") == i->npos) // A few advertisements (often for podcasts) change the window title. If the window title doesn't contain a dash it's an advertisement																   															   
				{																	
					if (FAILED(ChangeMuteStatus(processIDs, true)))					
						ErrorExit("Error while trying to mute/unmute Spotify");
					state = ADV;
				}
				else if (!ContainsUnicodeCharacters(*i)) // Check if song title does not unicode characters (cannot be displayed by most consoles)
				{
					if (FAILED(ChangeMuteStatus(processIDs, false)))
						ErrorExit("Error while trying to mute/unmute Spotify");
					songTitle = *i;
					state = SONG;
				}
				else // Song title contains unicode characters
				{
					if (FAILED(ChangeMuteStatus(processIDs, false)))
						ErrorExit("Error while trying to mute/unmute Spotify");
					state = SONGUNI;
				}
			}
		}
	}
	else // If title is empty, Spotify process couldn't be found
		state = NOTFOUND;
}

void Spotify::Clear()
{
	// Need to clear these or they will keep growing bigger with every call to EnumWindowsProc
	processIDs.clear();
	titles.clear();
}
