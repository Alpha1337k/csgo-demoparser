#include <demo.hpp>

int main(int argc, char **argv, char **env)
{
	(void)env;

	if (argc < 2)
		std::cout << "Error: filepath required" << std::endl;
	std::cout << argv[1] << std::endl;
	return (0);
}