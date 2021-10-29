#ifndef DEMFILE_HPP
#define DEMFILE_HPP

#include <vector>
#include <stdio.h>
#include <exception>
#include <byteswap.h>

struct DemHeader
{
	char 	headerDef[8];
	int		demoProtocol;
	unsigned int		networkProtocol;
	char	serverName[260];
	char	clientName[260];
	char	mapName[260];
	char	gameDir[260];
	float	demoLength;
	int		ticks;
	int		frames;
	int		signOnLength;

};

std::ostream &operator<<(std::ostream &o, const DemHeader &d);

//struct Frame
//{
//	int ServerFrame;
//	int ClientFrame; //ServerFrame and ClientFrame delta probably correspond to client ping.
//	int SubPacketSize;
//	*buffer = new char[SubPacketSize]; // State update message?
//	Packet pkt = (rest of frame as data exists) // All demo commands are strung together in this area, structure below
//	JunkData data = (unknown) // ex: 0x8f 5a b5 04 94 e6 7c 24 00 00 00 00 00 ... (40 bytes of 0x00 after the 0x24)
//								// This could either be the end of the frame or the start of the next frame.

//};

class DemoFile
{
private:
	DemHeader header;
	std::string	signOnData;
	//std::vector<Frame> frames;
public:
	DemoFile(FILE *f);
	~DemoFile();
};

#endif