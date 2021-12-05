#include <demo.hpp>

StartupParser startupParameters;

int main(int argc, char **argv, char **env)
{
	(void)env;

	startupParameters = StartupParser(argc, argv);
	if (argc < 2)
	{
		std::cerr << "Error: filepath required" << std::endl;
		return (-2);
	}
	FileReader f;

	if (f.load(startupParameters.last().first.c_str()) == -1)
	{
		std::cerr << "Error: file could not be opened" << std::endl;
		return (-1);
	}

	DemoFile demo(f);

	if (startupParameters["--only-parse"] == 0)
		demo.create_metrics();
	google::protobuf::ShutdownProtobufLibrary();
	return (0);
}