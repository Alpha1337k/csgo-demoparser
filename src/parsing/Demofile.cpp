#include <demo.hpp>

DemoFile::DemoFile()
{
	eventHooks.resize(50, 0);
}

void DemoFile::start_parse(FileReader &f)
{
	std::size_t headerSize;

	headerSize = f.read(&header, sizeof(header));
	if (headerSize != 1072)
		throw std::exception();
	bool isFinished = false;

	while (!isFinished)
	{
		Frame(f, isFinished, *this);
	}
}

DemoFile::~DemoFile()
{

}