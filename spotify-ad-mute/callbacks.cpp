#include "callbacks.hpp"
#include "spotify.hpp"
#include "utility.hpp"
#include <Psapi.h>

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM sptr)
{
	Spotify* spot = reinterpret_cast<Spotify*>(sptr);
	DWORD processID;
	TCHAR processName[MAX_PATH] = L"<unknown>";

	if (!GetWindowThreadProcessId(hWnd, &processID))
		ErrorExit("Error while getting process ID");

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	if (hProcess != NULL)
	{
		HMODULE hMod;
		DWORD needed;
		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &needed))
		{
			GetModuleBaseName(hProcess, hMod, processName, sizeof(processName) / sizeof(TCHAR));
		}
		std::wstring processNameStr(processName);

		if (processNameStr == L"Spotify.exe")
		{
			spot->AddProcessID(processID);

			int length = GetWindowTextLength(hWnd);
			if (length != 0)
			{
				TCHAR* buffer = new TCHAR[length + (long long)1];
				GetWindowText(hWnd, buffer, length + 1);
				std::wstring windowTitle(buffer);
				spot->AddTitle(windowTitle);
				delete[] buffer;

				if (spot->GetTitles().size() > 6)
					return FALSE;
			}
		}
	}

	return TRUE;
}
