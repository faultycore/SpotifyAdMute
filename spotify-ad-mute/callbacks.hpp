#pragma once
#pragma comment(lib, "psapi")

#include<iostream>
#include<Windows.h>
#include<Psapi.h>
#include "utility.hpp"

BOOL CALLBACK EnumWindowsProc(HWND, LPARAM);
