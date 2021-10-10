#pragma once

#include <string>
#include "InterfaceElement.h"

class InputBlock : public InterfaceElement
{
public:
	InputBlock(int x, int y, int width, int height);

	void print() override;

	void clear() override;

	bool hasInput() { return _input != ""; }

	const std::string& input() const { return _input; };
private:
	std::string _input;
};