#include "utility.h"

VOID ErrorExit(LPCSTR lpsErrMessage)
{
	std::cout << stderr << std::endl << lpsErrMessage;

	ExitProcess(0);
}

BOOL CheckForUnicodeCharacters(std::wstring str)
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