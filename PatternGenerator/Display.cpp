#include "Display.h"

Display::Display(void)
{
}

Display::~Display(void)
{
}

void Display::setDisplayArea(long bottom, long left, long right, long top)
{
	this->bottom = bottom;
	this->left = left;
	this->right = right;
	this->top = top;
}

void Display::setDisplayArea(RECT rect)
{
	this->rect = rect;
}

void Display::setName(TCHAR name[])
{
	this->name = name;
}

long Display::getBottom()
{
	return this->bottom;
}

long Display::getLeft()
{
	return this->left;
}

long Display::getRight()
{
	return this->right;
}

long Display::getTop()
{
	return this->top;
}

RECT Display::getDisplayArea()
{
	return this->rect;
}

std::string Display::getName()
{
	return this->name;
}