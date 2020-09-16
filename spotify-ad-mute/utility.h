#pragma once

#include <iostream>
#include <vector>
#include <Windows.h>

struct EnumData {
	DWORD dwProcessID[5]{ 0, 0, 0, 0, 0 };
	HWND hWnd = NULL;
	std::vector<std::wstring> title;
};

VOID ErrorExit(LPCSTR);
BOOL CheckForUnicodeCharacters(std::wstring);
