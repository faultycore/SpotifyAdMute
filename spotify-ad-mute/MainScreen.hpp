#pragma once
#include <chrono>
#include "Screen.hpp"

class MainScreen : public Screen
{
public:
	MainScreen(Data* data);

	virtual int handle_input(int key) override;

	virtual void print() override;
private:
	enum AnimState
	{
		POSE_01,
		POSE_02
	};

	std::chrono::time_point<std::chrono::steady_clock> _time;

	AnimState _animState;
};