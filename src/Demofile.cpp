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

std::ostream &operator<<(std::ostream &o, const Vector &d)
{
	o << "[ " << d.x << ", " << d.y << ", " << d.z << " ]";
	return o;
}

DemoFile::DemoFile(FILE *f)
{
	std::size_t headerSize;
	
	headerSize = fread(&header, 1, sizeof(header), f);
	if (headerSize != 1072)
		throw std::exception();
	std::cout << header << std::endl;
	bool isFinished = false;

	std::size_t packetCount = 0;
	while (!isFinished && packetCount < 100)
	{
		Frame frm(f, isFinished);
		std::cout << frm <<  std::endl;
		frames.push_back(frm);
		packetCount++;
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