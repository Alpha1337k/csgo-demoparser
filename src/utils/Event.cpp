#include <demo.hpp>

void	DemoFile::addEventHook(int type, std::function<void (void *)> &f)
{
	eventHooks[type] = f;
}

void	DemoFile::removeEventHook(int type)
{
	eventHooks[type] = 0;
}