#include <iostream>
#include "InputBlock.h"

InputBlock::InputBlock(int x, int y, int width, int height) : InterfaceElement(false, x, y, width, height),  _input("") {}

void InputBlock::print()
{
	if (_active)
	{
		position_cursor(0, 0);
		std::string in;
		std::cin >> in;
		_input = in;
		_active = false;
	}
}

void InputBlock::clear()
{
	_input = "";

	for (int i = 0; i < _height; i++)
	{
		std::cout << "\033[" + std::to_string(_y + i) + ";" + std::to_string(_x) + "f";
		std::cout << "\033[" + std::to_string(_width) + "X";
	}
}