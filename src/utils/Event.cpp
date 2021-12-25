#include <demo.hpp>

void	DemoFile::addEventHook(int type, void (*f)(void *data))
{
	eventHooks[type] = f;
}

void	DemoFile::removeEventHook(int type)
{
	eventHooks[type] = 0;
}