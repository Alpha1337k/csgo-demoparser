#include <demo.hpp>

void	DemoFile::addEventHook(SVC_Messages type, void (*f)(void *data))
{
	eventHooks[type] = f;
}

void	DemoFile::removeEventHook(SVC_Messages type)
{
	eventHooks[type] = 0;
}