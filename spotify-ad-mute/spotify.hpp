#pragma once

#include <vector>
#include <string>
#include <Windows.h>

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
	void UpdateSpotifyProcessInfo();
	void UpdateState();
	void Clear();

	void SetSongTitle(std::wstring songTitle) { this->songTitle = songTitle; }
	void AddProcessID(DWORD processID) { this->processIDs.push_back(processID); }
	void AddTitle(std::wstring title) { this->titles.push_back(title); }

	auto GetSongTitle()	const { return songTitle; }
	auto GetProcessIDs() const { return processIDs; }
	auto GetTitles() const { return titles; }
	auto GetTitlesIgnore() const { return titlesIgnore; }
	auto GetTitlesAdvert() const { return titlesAdvert; }
	auto GetTitlesNone() const { return titlesNone; }
	auto GetState() const { return state; }

private:
	std::vector<std::wstring> titlesIgnore;  // Window names to ignore											           
	std::vector<std::wstring> titlesAdvert;  // Window names that signify advertisement playing         
	std::vector<std::wstring> titlesNone;    // Window names that signify nothing playing

	std::wstring songTitle{};
	std::vector<DWORD> processIDs{};
	std::vector<std::wstring> titles{};

	int state;
};