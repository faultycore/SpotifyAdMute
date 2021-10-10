#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include "Data.hpp"
#include "InterfaceElement.h"

class Screen 
{
public:
	virtual ~Screen();

	virtual void print();

	virtual void force_reprint();

	virtual void clear_screen() const;

	virtual void add_text_block(int id, bool active, std::vector<std::wstring> text, int x, int y, int width, int height);

	virtual void add_input_block(int id, int x, int y, int width, int height);

	virtual const InterfaceElement* element(int id) const;

	virtual InterfaceElement* element(int id);

	virtual int handle_input(int key) = 0;
protected:
	Data* _data;
	std::map<int, InterfaceElement*> _elements;

	Screen(Data* data);
};