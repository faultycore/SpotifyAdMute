#pragma once

#include <list>
#include <functional>
#include <string>

template <typename... F>
class EventHandler
{
public:
	EventHandler(std::function<void(F...)> handlerFunc)
	{
		this->handlerFunc = handlerFunc;
	}

	void operator ()(F... params) const
	{
		if (handlerFunc)
		{
			handlerFunc(params...);
		}
	}

private:
	std::function<void(F...)> handlerFunc;
};

template <typename... F>
class Event
{
public:
	Event()
	{
	}

	~Event()
	{
		listeners.clear();
	}

	void Raise(F... params) const
	{
		for (const auto& handler : listeners)
		{
			handler(params...);
		}
	}

	void RegisterListener(EventHandler<F...> listener)
	{
		listeners.push_back(listener);
	}
private:
	std::list<EventHandler<F...>> listeners;
};


class EventTypes
{
public:
	static Event<int, std::wstring> SpotifyStateChangeEvent;
};