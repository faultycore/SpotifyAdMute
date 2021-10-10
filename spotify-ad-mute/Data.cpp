#include "Data.hpp"
#include "Spotify.hpp"
#include "Utility.hpp"

Data::Data()
{
	_mainBlock01 = { L"Press [esc] to exit", L"Press [o] for options / more keybinds", L"------------------------------------" };

	_mainBlock02 = {
		{SPOTERROR, {L"Error while checking for Spotify window" }},
		{NONE, {L"Playing: None" }},
		{ADV, {L"Playing: Advertisement :(" }},
		{SONG, {L"Playing: "}},
		{SONGUNI, {L"Unable to display song title" }},
		{NOTFOUND, {L"Unable to locate Spotify process..." }}
	};

	_mainBlock03 = { 
		{0, {
		L"                 ,)",
		L"             ,`/ |\\",
		L"             /  `/ |",
		L"           (,_,/  /",
		L"            `___, \\",
		L"                 \\ \\",
		L"                / , \\",
		L"               /,  \\ \\",
		L"               \\`   `.`.",
		L"                \\ \\   \\,`,",
		L"               -`-'    -'",
		L"" }},

		{1, {
		L"                    (,",
		L"               ,`/  /|",
		L"               /L',/ |",
		L"           ,)_,,\\', /",
		L"            `___,V |",
		L"                 \\ ,",
		L"                / , \\",
		L"               / , \\ \\",
		L"               \\`   `.`.",
		L"                \\ \\   \\,`,",
		L"               -`-'   -'",
		L"" }}
	};

	_mainBlock04 = { L"Credits: ejm @https://asciiart.website/" };

	_optionsBlock01 = { L"Press [esc] to exit", L"Press [o] again to get back", L"---------------------------" };

	_optionsBlock02 = {
		{ 
		L"Press the number associated with an option to change it.",
		L"KEYBINDS: ",
		L"Next track        [ctrl + right]",
		L"Previous track    [ctrl + left]",
		L"Pause/Unpause     [ctrl + space]",
		L"",
		L"OPTIONS: ",
		L"1. Interval in seconds",
		L"2. ASCII jam",
		L"3. Keybinds",
		L"4. Add title to blacklist" }
	};

	_optionsBlock04 = {
		{OPTION_NONE, {
		L""
		}},
		{OPTION_INTERVAL, {
		L"Enter wait period until the program updates changes to the state of Spotify [x] (in seconds): "
		}},
		{ OPTION_ASCIIJAM, {
		L"Should Ascii jam be enabled? [y/n]: "
		} },
		{ OPTION_NAME, {
		L""
		} },
		{ OPTION_KEYBINDS, {
		L""
		}}
	};

	_exitBlock01 = { L"exiting" };

	_titlesAd = { L"Advertisement", L"Spotify" };			        
	_titlesNone = { L"Spotify Free", L"Spotify Premium" };    
}

const BlockText& Data::mainBlock01() const
{
	return _mainBlock01;
}

const BlockText Data::mainBlock02(int spotifyState, std::wstring spotifyTitle) const
{
	if (spotifyState == SONG)
		return { _mainBlock02.at(SONG)[0] + L" " + spotifyTitle };
	else
		return _mainBlock02.at(spotifyState);
}

const BlockText& Data::mainBlock03(int anim) const
{
	return _mainBlock03.at(anim);
}

const BlockText& Data::mainBlock04() const
{
	return _mainBlock04;
}

const BlockText& Data::optionsBlock01() const
{
	return _optionsBlock01;
}

const BlockText& Data::optionsBlock02() const
{
	return _optionsBlock02;
}

const BlockText Data::optionsBlock03() const
{
	BlockText temp1;
	std::wstring temp2;

	temp1.push_back({ L"Current value: " + std::to_wstring(_options.interval) + L" seconds" });
	temp1.push_back({ L"Current value: " + std::wstring(_options.asciiJam == 1 ? L"enabled" : L"not enabled") });
	temp1.push_back({ L"" });
	temp1.push_back({ L"" });
	return temp1;
}

const BlockText& Data::optionsBlock04(int optionState) const
{
	return _optionsBlock04.at(optionState);	
}

const BlockText& Data::exitBlock01() const
{
	return _exitBlock01;
}

const BlockText& Data::titlesAd() const
{
	return _titlesAd;
}

const BlockText& Data::titlesNone() const
{
	return _titlesNone;
}