#include "OptionScreen.hpp"
#include "Data.hpp"
#include "Utility.hpp"
#include "InputBlock.h"
#include "TextBlock.h"

OptionScreen::OptionScreen(Data* data) : Screen(data), _optionState(OPTION_NONE)
{
	add_text_block(0, true, _data->optionsBlock01(), 5, 4, 10, 4);
	add_text_block(1, true, _data->optionsBlock02(), 5, 8, 40, 4);
	add_text_block(2, true, _data->optionsBlock03(), 40, 15, 40, 4);
	add_text_block(3, true, _data->optionsBlock04(OPTION_NONE), 5, 24, 100, 4);
	add_input_block(4, 5, 25, 20, 4);
}

void OptionScreen::print()
{
	Screen::print();

	if (dynamic_cast<InputBlock*>(element(4))->hasInput())
	{
		std::string s = (dynamic_cast<InputBlock*>(element(4))->input());

		switch (_optionState)
		{
		case OPTION_INTERVAL:
			_data->options().interval = std::atoi(s.c_str());
			break;
		case OPTION_ASCIIJAM:
			_data->options().asciiJam = (s == "y" || s == "n") ? (s == "y" ? true : false) : _data->options().asciiJam;
			break;
		default:
			break;
		}

		_optionState = OPTION_NONE;

		dynamic_cast<InputBlock*>(element(4))->deactivate();
		dynamic_cast<TextBlock*>(element(3))->deactivate();
		dynamic_cast<TextBlock*>(element(2))->change_text(_data->optionsBlock03());
		dynamic_cast<TextBlock*>(element(3))->change_text(_data->optionsBlock04(OPTION_NONE));	
	}
}

int OptionScreen::handle_input(int key)
{
	switch (key)
	{
	case VK_ESCAPE:
		return EXIT;
	case VK_O:
		dynamic_cast<TextBlock*>(element(3))->change_text(_data->optionsBlock04(OPTION_NONE));
		return MAIN;
		break;
	case VK_1:	
		_optionState = OPTION_INTERVAL;
		dynamic_cast<TextBlock*>(element(3))->change_text(_data->optionsBlock04(OPTION_INTERVAL));
		dynamic_cast<TextBlock*>(element(3))->activate();
		dynamic_cast<InputBlock*>(element(4))->activate();
		return OPTION;
		break;
	case VK_2:
		_optionState = OPTION_ASCIIJAM;
		dynamic_cast<TextBlock*>(element(3))->change_text(_data->optionsBlock04(OPTION_ASCIIJAM));
		dynamic_cast<TextBlock*>(element(3))->activate();
		dynamic_cast<InputBlock*>(element(4))->activate();
		return OPTION;
		break;
	case VK_3:
		_optionState = OPTION_KEYBINDS;
		dynamic_cast<TextBlock*>(element(3))->change_text(_data->optionsBlock04(OPTION_KEYBINDS));
		dynamic_cast<TextBlock*>(element(3))->activate();
		return OPTION;
		break;
	case VK_4:
		_optionState = OPTION_NAME;
		dynamic_cast<TextBlock*>(element(3))->change_text(_data->optionsBlock04(OPTION_NAME));
		dynamic_cast<TextBlock*>(element(3))->activate();
		return OPTION;
		break;
	default:
		return NO_CHANGE;
	}
}