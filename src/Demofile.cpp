#include <demo.hpp>

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
	size_t size = fread(&header, 1, sizeof(header), f);
	if (size != 1072)
		throw std::exception();
	std::cout << header << std::endl;
	bool isFinished = false;

	while (!isFinished)
	{
		Frame frm(f, isFinished);
		std::cout << frm << std::endl;
		frames.push_back(frm);
	}
	
	//signOnData.reserve(header->signOnLength);
	//size_t read = fread(&signOnData[0], sizeof(char) * header->signOnLength, 1, f);

	//if (read == 0)
	//{
	//	std::cout << "error! readsignon" << std::endl;
	//}
	//std::cout << "SignOndata: " << signOnData << std::endl;
	//frames.reserve(header->ticks);
}

DemoFile::~DemoFile()
{

}