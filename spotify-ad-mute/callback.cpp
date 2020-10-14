#include "callback.h"

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM sptr)
{
	EnumData* spot = reinterpret_cast<EnumData*>(sptr);
	DWORD dwProcessID;
	TCHAR szProcessName[MAX_PATH] = L"<unknown>";

	if (!GetWindowThreadProcessId(hWnd, &dwProcessID))
	{
		ErrorExit("Getting process ID");
		return TRUE;
	}

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessID);
	if (hProcess != NULL)
	{
		HMODULE hMod;
		DWORD cbNeeded;
		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
		{
			GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
		}
		std::wstring swProcessName(szProcessName);

		if (swProcessName == L"Spotify.exe")
		{
			if (spot->dwProcessID[0] == 0)
				spot->dwProcessID[0] = dwProcessID;
			else if (spot->dwProcessID[1] == 0)
				spot->dwProcessID[1] = dwProcessID;
			else if (spot->dwProcessID[2] == 0)
				spot->dwProcessID[2] = dwProcessID;
			else if (spot->dwProcessID[3] == 0)
				spot->dwProcessID[3] = dwProcessID;
			else if (spot->dwProcessID[4] == 0)
				spot->dwProcessID[4] = dwProcessID;
		}
	}

	if (spot->dwProcessID[0] == dwProcessID || spot->dwProcessID[1] == dwProcessID || spot->dwProcessID[2] == dwProcessID ||
		spot->dwProcessID[3] == dwProcessID || spot->dwProcessID[4] == dwProcessID)
	{
		

		int length = GetWindowTextLength(hWnd);
		if (length != 0)
		{
			TCHAR* buffer = new TCHAR[length + (long long)1];
			GetWindowText(hWnd, buffer, length + 1);
			std::wstring windowTitle(buffer);
			spot->title.push_back(windowTitle);

			if (windowTitle != L"GDI+ Window (Spotify.exe)" && windowTitle != L"MSCTFIME UI" && windowTitle != L"Default IME" && windowTitle != L"CSpNotify Notify Window" && windowTitle != L"G")
				spot->hWnd = hWnd;

			delete[] buffer;
		}

		if (spot->title.size() >= 6)
			return FALSE;
	}
	return TRUE;
}

WORD KeyEventProc(KEY_EVENT_RECORD ker, WORD* ctrlKey)
{
	if (ker.bKeyDown)
	{
		*ctrlKey = ker.dwControlKeyState;
		return ker.wVirtualKeyCode;
	}
	else return 0;
}