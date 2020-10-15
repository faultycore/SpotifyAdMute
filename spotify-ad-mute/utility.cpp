#include "utility.hpp"

VOID ErrorExit(LPCSTR lpsErrMessage)
{
	std::cout << stderr << std::endl << lpsErrMessage;

	ExitProcess(0);
}

BOOL ContainsUnicodeCharacters(std::wstring str)
{
	size_t length = str.size();

	for (size_t i = 0; i < length; ++i)
	{
		size_t c = (size_t)str.at(i);

		if (c > 255)
			return true;
	}
	return false;
}

BOOL Contains(std::vector<std::wstring> v, std::wstring str)
{
	std::vector<std::wstring>::iterator it = find(v.begin(), v.end(), str);

	if (it != v.end())
		return true;
	return false;
}

WORD GetKeyEvent(KEY_EVENT_RECORD ker, WORD* ctrlKey)
{
	if (ker.bKeyDown)
	{
		*ctrlKey = ker.dwControlKeyState;
		return ker.wVirtualKeyCode;
	}
	else return 0;
}

void ShowOptions()
{

}