#include "Callbacks.hpp"
#include "Spotify.hpp"
#include "Utility.hpp"
#include <Psapi.h>

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM sptr)
{
	Spotify* data = reinterpret_cast<Spotify*>(sptr);
	DWORD processId;
	HANDLE handle;

	if (!GetWindowThreadProcessId(hWnd, &processId))
		error_exit("Error while getting process ID");

	if (IsWindowVisible(hWnd))
	{
		handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
		if (handle)
		{
			wchar_t processName[50];
			if (!GetModuleBaseName(handle, 0, processName, 50))
			{
				error_exit("Failed to retrieve module name");
			}

			std::wstring name(processName);

			if (name == L"Spotify.exe")
			{
				int length = GetWindowTextLength(hWnd);
				if (length != 0)
				{
					TCHAR* buffer = new TCHAR[length + (long long)1];
					GetWindowText(hWnd, buffer, length + 1);
					std::wstring windowTitle(buffer);
					data->title = windowTitle;
					data->procID = processId;
					delete[] buffer;

					return FALSE;
				}
			}

				CloseHandle(handle);
		}
	}
	
	return TRUE;
}
