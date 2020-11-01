#include "spotify.hpp"
#include "callbacks.hpp"
#include "utility.hpp"
#include "audiocom.hpp"
#include <combaseapi.h>

Spotify::Spotify()
{
	titlesAdvert.insert(titlesAdvert.end(), { L"Advertisement", L"Spotify" }); // Window names that signify advertisement playing  			
	titlesNone.insert(titlesNone.end(), { L"Spotify Free", L"Spotify Premium" }); // Window names that signify nothing playing

	pAudioVolume = NULL;
	processID = 0;
	state = -1;
}

Spotify::~Spotify()
{
	SafeRelease(&pAudioVolume);
}

void Spotify::HookSpotify()
{
	EnumAudioSessions(&pAudioVolume, &processID);
}

void Spotify::UpdateSpotifyProcessInfo()
{
	EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(this));
}

void Spotify::UpdateState()
{
		if (spotifyTitle != L"")
		{ 
			if (Contains(titlesNone, spotifyTitle)) // Check if nothing is playing
				state = NONE;
			else if (Contains(titlesAdvert, spotifyTitle)) // Check if an advert is playing
			{
				if (FAILED(ChangeMuteStatus(this->pAudioVolume, true)))
					ErrorExit("Error while trying to mute/unmute Spotify");
				state = ADV;
			}
			else if (spotifyTitle.find(L"-") == spotifyTitle.npos) // A few advertisements (often for podcasts) change the window title. If the window title doesn't contain a dash it's an advertisement																   															   
			{
				if (FAILED(ChangeMuteStatus(this->pAudioVolume, true)))
					ErrorExit("Error while trying to mute/unmute Spotify");
				state = ADV;
			}
			else if (!ContainsUnicodeCharacters(spotifyTitle)) // Check if song title does not unicode characters (cannot be displayed by most consoles)
			{
				if (FAILED(ChangeMuteStatus(this->pAudioVolume, false)))
					ErrorExit("Error while trying to mute/unmute Spotify");
				state = SONG;
			}
			else // Song title contains unicode characters
			{
				if (FAILED(ChangeMuteStatus(this->pAudioVolume, false)))
					ErrorExit("Error while trying to mute/unmute Spotify");
				state = SONGUNI;
			}
		}	
		else // If title is empty, Spotify process couldn't be found
			state = NOTFOUND;
}
