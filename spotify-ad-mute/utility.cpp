#include "Utility.hpp"

void error_exit(LPCSTR lpsErrMessage)
{
	std::cout << stderr << std::endl << lpsErrMessage;

	ExitProcess(0);
}

bool contains_unicode(std::wstring str)
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

bool contains(std::vector<std::wstring> v, std::wstring str)
{
	std::vector<std::wstring>::iterator it = find(v.begin(), v.end(), str);

	if (it != v.end())
		return true;
	return false;
}

WORD get_key_event(KEY_EVENT_RECORD ker, WORD* ctrlKey)
{
	if (ker.bKeyDown)
	{
		*ctrlKey = ker.dwControlKeyState;
		return ker.wVirtualKeyCode;
	}
	else return 0;
}


