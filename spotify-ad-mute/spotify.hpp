#pragma once

#include <vector>
#include <string>
#include <Windows.h>
#include <audiopolicy.h>

class Spotify
{
public:
	static const unsigned short SPOTERROR = -1;
	static const unsigned short NONE = 0;
	static const unsigned short ADV = 1;
	static const unsigned short SONG = 2;
	static const unsigned short SONGUNI = 3;
	static const unsigned short NOTFOUND = 4;

	Spotify();
	~Spotify();
	void HookSpotify();
	void UpdateSpotifyProcessInfo();
	void UpdateState();

	void SetSpotifyTitle(std::wstring spotifyTitle) { this->spotifyTitle = spotifyTitle; }
	void SetProcessID(DWORD processID) { this->processID = processID; }

	auto GetSpotifyTitle()	const { return spotifyTitle; }
	auto GetProcessID() const { return processID; }
	auto GetTitlesAdvert() const { return titlesAdvert; }
	auto GetTitlesNone() const { return titlesNone; }
	auto GetState() const { return state; }

private:										           
	std::vector<std::wstring> titlesAdvert;  // Window names that signify advertisement playing         
	std::vector<std::wstring> titlesNone;    // Window names that signify nothing playing

	DWORD processID;
	std::wstring spotifyTitle;

	ISimpleAudioVolume* pAudioVolume;

	int state;
};