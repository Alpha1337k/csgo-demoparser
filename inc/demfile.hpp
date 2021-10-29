#ifndef DEMFILE_HPP
#define DEMFILE_HPP

#include <vector>
#include <stdio.h>
#include <exception>

struct DemHeader
{
	char 	headerDef[8];
	int		demoProtocol;
	int		networkProtocol;
	char	serverName[260];
	char	clientName[260];
	char	mapName[260];
	char	gameDir[260];
	float	demoLength;
	int		ticks;
	int		frames;
	int		signOnLength;

	DemHeader(FILE *f);
};

std::ostream &operator<<(std::ostream &o, const DemHeader &d);

struct Frame
{
	
};

class DemoFile
{
private:
	DemHeader *header;
	std::vector<Frame> frames;
public:
	DemoFile(FILE *f);
	~DemoFile();
};

#endif