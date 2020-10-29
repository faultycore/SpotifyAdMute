#pragma once
#pragma comment(lib, "psapi")

#include<Windows.h>

BOOL CALLBACK EnumWindowsProc(HWND, LPARAM);
