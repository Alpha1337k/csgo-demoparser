#include <demo.hpp>

int main(int argc, char **argv, char **env)
{
	(void)env;

	if (argc < 2)
		std::cout << "Error: filepath required" << std::endl;
	std::cout << argv[1] << std::endl;
	FILE *f = fopen(argv[1], "r");
	DemoFile demo(f);

	fclose(f);
	return (0);
}