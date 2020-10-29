#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <Windows.h>

void ErrorExit(LPCSTR);
bool ContainsUnicodeCharacters(std::wstring);
bool Contains(std::vector<std::wstring>, std::wstring);
WORD GetKeyEvent(KEY_EVENT_RECORD, WORD*);
void LoadConfig();
void SaveConfig();
