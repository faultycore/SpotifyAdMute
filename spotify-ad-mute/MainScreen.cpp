#include "MainScreen.hpp"
#include "Data.hpp"
#include "Event.hpp"
#include "Utility.hpp"
#include "TextBlock.h"

MainScreen::MainScreen(Data* data) : Screen(data), _animState(POSE_02)
{
	_time = std::chrono::steady_clock::now();
	add_text_block(0, true, _data->mainBlock01(), 5, 4, 10, 4);
	add_text_block(1, true, _data->mainBlock02(NONE, L""), 5, 8, 80, 4);
	add_text_block(2, false, _data->mainBlock03(_animState), 5, 16, 30, 10);
	add_text_block(3, false, _data->mainBlock04(), 5, 28, 30, 1);

	EventTypes::SpotifyStateChangeEvent.RegisterListener(EventHandler<int, std::wstring>([this](int spotifyState, std::wstring title) {
		dynamic_cast<TextBlock*>(element(1))->change_text(_data->mainBlock02(spotifyState, title));
	}));
}

void MainScreen::print()
{
	Screen::print();

	if (!dynamic_cast<TextBlock*>(element(2))->active() && _data->options().asciiJam)
	{
		dynamic_cast<TextBlock*>(element(2))->activate();
		dynamic_cast<TextBlock*>(element(3))->activate();
	} 
	else if (dynamic_cast<TextBlock*>(element(2))->active() && !_data->options().asciiJam)
	{
		dynamic_cast<TextBlock*>(element(2))->deactivate();
		dynamic_cast<TextBlock*>(element(3))->deactivate();
	}

	if (_data->options().asciiJam)
	{
		auto timeNew = std::chrono::steady_clock::now();
		if ((timeNew - _time) >= std::chrono::milliseconds(500))
		{
			_time = timeNew;
			_animState = (AnimState)(1 - _animState);
			dynamic_cast<TextBlock*>(element(2))->change_text(_data->mainBlock03(_animState));
		}
	}
}

int MainScreen::handle_input(int key) 
{
	switch (key)
	{
	case VK_ESCAPE:
		return EXIT;
	case VK_O:
		return OPTION;
		break;
	default:
		return NO_CHANGE;
	}
}