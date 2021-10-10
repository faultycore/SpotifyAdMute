#pragma once

#include <vector>
#include <string>
#include <audiopolicy.h>
#include "Data.hpp"

class Spotify
{
public:
	std::wstring title;
	DWORD procID;

	Spotify(Data *data);

	~Spotify();

	void update_spotify_proc_info();

	void update_state();

	int& state() { return _state; };

private:
	std::vector<std::wstring> _titlesAd;
	std::vector<std::wstring> _titlesNone;

	ISimpleAudioVolume* _pAudioVolume;

	int _state;

	Data* _data;

	void hook_spotify();
};