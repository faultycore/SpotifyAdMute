#include "ExitScreen.h"
#include "Utility.hpp"

ExitScreen::ExitScreen(Data* data) : Screen(data) {}

int ExitScreen::handle_input(int key)
{
	return EXIT;
}