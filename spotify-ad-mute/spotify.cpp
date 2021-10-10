#include "Spotify.hpp"
#include "Callbacks.hpp"
#include "Utility.hpp"
#include "Audiocom.hpp"
#include "Event.hpp"

Spotify::Spotify(Data *data) : _data(data)
{
	_titlesAd = data->titlesAd();				//Window names meaning advertisement is playing         
	_titlesNone = data->titlesNone();			//Window names meaning nothing is playing

	_pAudioVolume = NULL;

	_state = NONE;
}

Spotify::~Spotify()
{
	safe_release(&_pAudioVolume);
}

void Spotify::hook_spotify()
{
	enum_audio_sessions(&_pAudioVolume);
}

void Spotify::update_spotify_proc_info()
{
	EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(this));
}

void Spotify::update_state()
{
	if (title != L"")
	{
		//Check if nothing is playing
		if (contains(_titlesNone, title))
		{
			_state = NONE;
			EventTypes::SpotifyStateChangeEvent.Raise(_state, title);
		}

		//Check if an advert is playing
		else if (contains(_titlesAd, title))
		{
			if (_pAudioVolume == NULL)
				enum_audio_sessions(&_pAudioVolume);

			if (FAILED(change_mute_status(_pAudioVolume, true)))
				error_exit("Error while trying to mute/unmute Spotify");
			_state = ADV;
			EventTypes::SpotifyStateChangeEvent.Raise(_state, title);
		}

		//Hack to try and catch really all advertisments
		else if (title.find(L"-") == title.npos)
		{
			if (_pAudioVolume == NULL)
				enum_audio_sessions(&_pAudioVolume);
			
			if (FAILED(change_mute_status(_pAudioVolume, true)))
				error_exit("Error while trying to mute/unmute Spotify");
			_state = ADV;
			EventTypes::SpotifyStateChangeEvent.Raise(_state, title);
		}

		//Check if song title does not unicode characters (cannot be displayed by most consoles)
		else if (!contains_unicode(title))
		{
			if (_pAudioVolume == NULL)
				enum_audio_sessions(&_pAudioVolume);

			if (FAILED(change_mute_status(_pAudioVolume, false)))
				error_exit("Error while trying to mute/unmute Spotify");
			_state = SONG;
			EventTypes::SpotifyStateChangeEvent.Raise(_state, title);
		}

		//Song title contains unicode characters
		else
		{
			if (_pAudioVolume == NULL)
				enum_audio_sessions(&_pAudioVolume);

			if (FAILED(change_mute_status(_pAudioVolume, false)))
				error_exit("Error while trying to mute/unmute Spotify");
			_state = SONGUNI;
			EventTypes::SpotifyStateChangeEvent.Raise(_state, title);
		}
	}
	//If title is empty, Spotify process couldn't be found
	else
	{
		safe_release(&_pAudioVolume);

		_state = NOTFOUND;
		EventTypes::SpotifyStateChangeEvent.Raise(_state, title);
	}
}

