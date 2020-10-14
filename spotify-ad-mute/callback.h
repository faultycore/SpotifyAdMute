#pragma once
#pragma comment(lib, "psapi")

#include<iostream>
#include<Windows.h>
#include<Psapi.h>
#include "utility.h"

BOOL CALLBACK EnumWindowsProc(HWND, LPARAM);
WORD KeyEventProc(KEY_EVENT_RECORD, WORD*);