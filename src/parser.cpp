#include <demo.hpp>

DemHeader::DemHeader(FILE *f)
{
	size_t read = 0;

	read += fread(&headerDef, sizeof(headerDef), 1, f);
	read += fread(&demoProtocol, sizeof(demoProtocol), 1, f);
	read += fread(&networkProtocol, sizeof(networkProtocol), 1, f);
	read += fread(&serverName, sizeof(serverName), 1, f);
	read += fread(&clientName, sizeof(clientName), 1, f);
	read += fread(&mapName, sizeof(mapName), 1, f);
	read += fread(&gameDir, sizeof(gameDir), 1, f);
	read += fread(&demoLength, sizeof(demoLength), 1, f);
	read += fread(&ticks, sizeof(ticks), 1, f);
	read += fread(&frames, sizeof(frames), 1, f);
	read += fread(&signOnLength, sizeof(signOnLength), 1, f);

	if (read != 11)
		throw new std::length_error("header file corrupted");
}

std::ostream &operator<<(std::ostream &o, const DemHeader &d)
{
	o << "DemHeader {\n";
	o << "headerDef:       " << d.headerDef << std::endl;
	o << "demoProtocol:    " << d.demoProtocol << std::endl;
	o << "networkProtocol: " << d.networkProtocol << std::endl;
	o << "serverName:      " << d.serverName << std::endl;
	o << "clientName:      " << d.clientName << std::endl;
	o << "mapName:         " << d.mapName << std::endl;
	o << "gameDir:         " << d.gameDir << std::endl;
	o << "demoLength:      " << d.demoLength << std::endl;
	o << "ticks:           " << d.ticks << std::endl;
	o << "frames:          " << d.frames << std::endl;
	o << "signOnLength:    " << d.signOnLength << std::endl;

	o << "}\n";

	return (o);
}

DemoFile::DemoFile(FILE *f)
{

	header = new DemHeader(f);
	std::cout << *header << std::endl;

	frames.reserve(header->ticks);
}

DemoFile::~DemoFile()
{
	delete header;
}