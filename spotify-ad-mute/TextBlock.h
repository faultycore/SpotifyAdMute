#pragma once

#include <map>
#include <vector>
#include <string>
#include "InterfaceElement.h"

class TextBlock : public InterfaceElement
{
public:
	TextBlock(bool active, std::vector<std::wstring> text, int x, int y, int width, int height);

	void change_text(std::vector<std::wstring> text);

	void change_text(std::wstring text);

	void clear() override;

	void print() override;
private:
	std::vector<std::wstring> _text;
};