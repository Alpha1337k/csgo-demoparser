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

	FILE *f = fopen(startupParameters.last().first.c_str(), "r");
	if (f == NULL)
	{
		std::cerr << "Error: file could not be opened" << std::endl;
		return (-1);
	}
	DemoFile demo(f);

	if (startupParameters["--only-parse"] == 0)
		demo.create_metrics();
	fclose(f);
	return (0);
}