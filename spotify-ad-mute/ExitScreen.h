#pragma once

#include "Screen.hpp"
#include "Data.hpp"

class ExitScreen : public Screen
{
public:
	ExitScreen(Data* data);

	virtual int handle_input(int key) override;
};
