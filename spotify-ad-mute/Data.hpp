#pragma once

#include <map>
#include <vector>
#include <string>

typedef std::vector<std::wstring> BlockText;
typedef std::map<int, BlockText> BlockTextMap;

class Data {
public:
	Data(); 

	struct Options
	{
		unsigned short interval = 2;
		bool asciiJam = false;
	};

	const BlockText& mainBlock01() const;

	const BlockText mainBlock02(int spotifyState, std::wstring spotifyTitle) const;

	const BlockText& mainBlock03(int anim) const;

	const BlockText& mainBlock04() const;

	const BlockText& optionsBlock01() const;

	const BlockText& optionsBlock02() const;

	const BlockText optionsBlock03() const;

	const BlockText& optionsBlock04(int optionState) const;

	const BlockText& exitBlock01() const;

	const BlockText& titlesAd() const;

	const BlockText& titlesNone() const;

	Options& options() { return _options; };

private:
	BlockText _mainBlock01;
	BlockTextMap _mainBlock02;
	BlockTextMap _mainBlock03;
	BlockText _mainBlock04;
	BlockText _optionsBlock01;
	BlockText _optionsBlock02;
	BlockText _optionsBlock03;
	BlockTextMap _optionsBlock04;
	BlockText _exitBlock01;

	BlockText _titlesAd;
	BlockText _titlesNone;

	Options _options;
};

