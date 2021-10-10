#include <iostream>
#include "TextBlock.h"
#include "Event.hpp"

TextBlock::TextBlock(bool active, std::vector<std::wstring> text, int x, int y, int width, int height) : InterfaceElement(active, x, y, width, height), _text(text) {}

void TextBlock::change_text(std::vector<std::wstring> text)
{
	this->_text.clear();
	this->_text = text;
	this->_needsUpdated = true;
}

void TextBlock::change_text(std::wstring text)
{
	this->_text.clear();
	this->_text.push_back(text);
	this->_needsUpdated = true;
}

void TextBlock::clear()
{
	for (int i = 0; i < _height; i++)
	{
		std::cout << "\033[" + std::to_string(_y + i) + ";" + std::to_string(_x) + "f";
		std::cout << "\033[" + std::to_string(_width) + "X";
	}
}

void TextBlock::print() 
{
	this->clear();
	if (_active)
	{
		int c = 0;
		for (auto& t : _text)
		{
			position_cursor(0, c++);
			std::wcout << t;
		}
	}

	this->_needsUpdated = false;
}
