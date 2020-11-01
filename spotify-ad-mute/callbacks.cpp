#include "callbacks.hpp"
#include "spotify.hpp"
#include "utility.hpp"
#include <Psapi.h>

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM sptr)
{
	Spotify* spot = reinterpret_cast<Spotify*>(sptr);
	DWORD processID;

	if (!GetWindowThreadProcessId(hWnd, &processID))
		ErrorExit("Error while getting process ID");

	if ((processID == spot->GetProcessID()) && IsWindowVisible(hWnd))
	{
			int length = GetWindowTextLength(hWnd);
			if (length != 0)
			{
				TCHAR* buffer = new TCHAR[length + (long long)1];
				GetWindowText(hWnd, buffer, length + 1);
				std::wstring windowTitle(buffer);
				spot->SetSpotifyTitle(windowTitle);
				delete[] buffer;

				return FALSE;
			}
	}
	
	return TRUE;
}
