#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <Windows.h>

static const WORD VK_O = 0x4F;
static const WORD VK_A = 0x41;
static const WORD VK_X = 0x58;
static const WORD VK_Z = 0x5A;
static const WORD VK_1 = 0x31;
static const WORD VK_2 = 0x32;
static const WORD VK_3 = 0x33;
static const WORD VK_4 = 0x34;
static const WORD VK_5 = 0x35;
static const WORD VK_6 = 0x36;

enum SpotifyState
{
	SPOTERROR,
	NONE,
	ADV,
	SONG,
	SONGUNI,
	NOTFOUND
};

enum InterfaceState
{
	NO_CHANGE,
	MAIN,
	OPTION,
	EXIT
};

enum OptionState
{
	OPTION_NONE,
	OPTION_INTERVAL,
	OPTION_ASCIIJAM,
	OPTION_NAME,
	OPTION_KEYBINDS
};

void error_exit(LPCSTR);

bool contains_unicode(std::wstring);

bool contains(std::vector<std::wstring>, std::wstring);

WORD get_key_event(KEY_EVENT_RECORD, WORD*);


