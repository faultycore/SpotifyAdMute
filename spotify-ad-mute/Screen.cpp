#include <iostream>
#include "Screen.hpp"
#include "TextBlock.h"
#include "InputBlock.h"

Screen::Screen(Data* data) : _data(data) {}

Screen::~Screen()
{
	for (auto e : _elements)
		delete e.second;
}

void Screen::add_text_block(int id, bool active, std::vector<std::wstring> text, int x, int y, int width, int height)
{
	_elements.insert(std::pair<int, InterfaceElement*>(id, new TextBlock(active, text, x, y, width, height)));
}

void Screen::add_input_block(int id, int x, int y, int width, int height)
{
	_elements.insert(std::pair<int, InterfaceElement*>(id, new InputBlock(x, y, width, height)));
}

const InterfaceElement* Screen::element(int id) const
{
	return _elements.at(id);
}

InterfaceElement* Screen::element(int id)
{
	return _elements.at(id);
}

void Screen::print()
{
	for (auto e : _elements)
	{
		if (e.second->needsUpdated())
			e.second->print();
	}
}

void Screen::force_reprint()
{
	clear_screen();

	for (auto e : _elements)
	{
		e.second->print();
	}
}

void Screen::clear_screen() const
{
	std::wcout << "\033[2;0f";
	for (int i = 0; i < 30; i++)
		std::wcout << "\033[K" << std::endl;
}

