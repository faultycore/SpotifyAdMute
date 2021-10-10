#pragma once

class InterfaceElement
{
public:
	virtual void print() = 0;

	virtual void clear() = 0;

	virtual void activate();

	virtual void deactivate();

	const bool& active() const { return _active; };

	const bool& needsUpdated() const { return _needsUpdated; };
protected:
	int _x, _y, _width, _height;
	bool _active;
	bool _needsUpdated;

	InterfaceElement(bool active, int x, int y, int width, int height);

	void position_cursor(int x, int y) const;
};