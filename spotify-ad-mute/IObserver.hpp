#pragma once

#include <string>

class IObserver
{
public:
	virtual ~IObserver();

	virtual void Update(int eventType) = 0;
};