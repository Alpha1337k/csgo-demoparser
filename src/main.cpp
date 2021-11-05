#include <demo.hpp>

int main(int argc, char **argv, char **env)
{
	(void)env;

	if (argc < 2)
	{
		std::cout << "Error: filepath required" << std::endl;
		return (-1);
	}

	FILE *f = fopen(argv[1], "r");
	DemoFile demo(f);

	demo.create_metrics();
	fclose(f);
	return (0);
}