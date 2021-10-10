#pragma once

#include "Screen.hpp"

class OptionScreen : public Screen
{
public:
	OptionScreen(Data* data);

	virtual int handle_input(int key) override;

	virtual void print() override;
private:
	int _optionState;
};