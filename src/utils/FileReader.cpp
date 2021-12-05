#include <demo.hpp>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

FileReader::~FileReader() {}

FileReader::FileReader(std::string path): idx(0)
{
	if (load(path) == -1)
		throw std::invalid_argument("File cannot be opened");
}

int				FileReader::load(std::string path)
{
	idx = 0;
	data.clear();
	struct stat s;

	stat(path.c_str(), &s);

	data.resize(s.st_size);

	int fd = open(path.c_str(), O_RDONLY);
	if (fd == -1)
		return (-1);

	::read(fd, &data[0], s.st_size);

	close(fd);
	return (0);
}

inline size_t	FileReader::read(void *buffer, size_t size)
{
	if (size + idx > data.length())
		size = data.length() - idx;
	memcpy(buffer, &data[idx], size);
	idx += size;

	return size;
}

inline bool		FileReader::isEof()
{
	return idx >= data.length();
}