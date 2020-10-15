#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <Windows.h>

struct EnumData {
	DWORD dwProcessID[5]{ 0, 0, 0, 0, 0 };
	int cDWProcessIDSize = 5;
	HWND hWnd = NULL;
	std::vector<std::wstring> title{};
};

VOID ErrorExit(LPCSTR);
BOOL ContainsUnicodeCharacters(std::wstring);
BOOL Contains(std::vector<std::wstring>, std::wstring);
WORD GetKeyEvent(KEY_EVENT_RECORD, WORD*);
VOID ShowOptions( /* TODO: IMPLEMENT*/ );

