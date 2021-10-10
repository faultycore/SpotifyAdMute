#pragma once

#include "IObserver.hpp"

class IObservable 
{
public:
	virtual ~IObservable();

	virtual void Notify(int eventType) = 0;

	virtual void Attach(IObserver* obs) = 0;

	virtual void Detach(IObserver* obs) = 0;
};