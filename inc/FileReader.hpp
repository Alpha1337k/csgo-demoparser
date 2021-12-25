#ifndef FILEREADER_HPP
#define FILEREADER_HPP

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>

class FileReader
{
private:
	std::string data;
	size_t		idx;
public:
	FileReader(): idx(0) {}
	FileReader(std::string path): idx(0)
	{
		if (load(path) == -1)
			throw std::invalid_argument("File cannot be opened");		
	}
	~FileReader() {}

	int				load(std::string path)
	{
		idx = 0;
		data.clear();
		struct stat s;

		if (stat(path.c_str(), &s) == -1)
			return -1;

		data.resize(s.st_size);

		int fd = open(path.c_str(), O_RDONLY);
		if (fd == -1)
			return (-1);

		::read(fd, &data[0], s.st_size);

		close(fd);
		return (0);
	}

	int				loadFd(int fd)
	{
		idx = 0;
		data.clear();
		int ret = 1;
		std::string buffer;

		buffer.resize(4096);

		while (ret)
		{
			ret = ::read(fd, &buffer[0], 4096);

			if (ret > 0)
			{
				data += buffer.substr(0, ret);
			}
		}
		return ret;
	}

	size_t			read(void *buffer, size_t size)
	{
		if (size + idx > data.length())
			size = data.length() - idx;
		memcpy(buffer, &data[idx], size);
		idx += size;

		return size;
	}

	inline bool			isEof()
	{
		return idx >= data.length();
	}

	inline char			*getCurrentChar()
	{
		return &(data[idx]);
	}

	inline void			ForceIncrement(size_t len)
	{
		idx += len;
	}

	inline int				readInt()
	{
		unsigned int result = 0;
		char b = 0;
		char count = 0;
		do
		{
			if (count == 5)
			{
				return result;
			}
			read(&b, 1);
			result |= (b & 127) << (7 * count);
			count++;
		} while (b & 128);
		return result;
	}

	inline std::string		readString()
	{
		std::string rv;
		char tmp;

		while (1)
		{
			read(&tmp, 1);
			if (tmp == 0)
				break;
			rv += tmp;
		}
		
		return rv;	
	}
	inline size_t			getOffset()
	{
		return idx;
	}
};



#endif