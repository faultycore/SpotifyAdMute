#include <iostream>
#include <string>
#include "InterfaceElement.h"

InterfaceElement::InterfaceElement(bool active, int x, int y, int width, int height) : _active(active), _x(x), _y(y), _width(width), _height(height), _needsUpdated(true) {}

void InterfaceElement::position_cursor(int x, int y) const
{
	//std::cout << "\033[" + std::to_string(_y + y) + ";" + std::to_string(_x + x) + "f";
	std::cout << "\033[" + std::to_string(_y + y) + "d";
	std::cout << "\033[" + std::to_string(_x + x) + "G";
}

void InterfaceElement::activate()
{
	_active = true;
	_needsUpdated = true;
}

void InterfaceElement::deactivate()
{
	_active = false;
	this->clear();
}