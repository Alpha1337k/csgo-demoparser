#include <demo.hpp>

Packet::Packet(FILE *f)
{

	fread(this, 1, sizeof(*this), f);

	int sequenceIn = 0, sequenceOut = 0;
	size_t size = 0;
	unsigned int chunkSize = 0;
	size = fread(&sequenceIn, sizeof(int), 1, f);
	size = fread(&sequenceOut, sizeof(int), 1, f);
	//std::cout << "sequence: " << sequenceIn << ", " << sequenceOut << std::endl;
	fread(&chunkSize, sizeof(int), 1, f);
	std::cout << "chunkSize: " << chunkSize << std::endl;
	MessageVector messages = getProtoMesssages(f, chunkSize);
	 
}

std::ostream &operator<<(std::ostream &o, const Packet &p)
{
	o << "Packet {\n";
	o << "flags:            " << p.splits[0].flags << std::endl;
	o << "viewOrigin:       " << p.splits[0].viewOrigin << std::endl;
	o << "viewAngles:       " << p.splits[0].viewAngles << std::endl;
	o << "localViewAngles:  " << p.splits[0].localViewAngles << std::endl;
	o << "viewOrigin:       " << p.splits[0].viewOrigin2 << std::endl;
	o << "viewAngles:       " << p.splits[0].viewAngles2 << std::endl;
	o << "localViewAngles:  " << p.splits[0].localViewAngles2 << std::endl;
	o << "flags:            " << p.splits[1].flags << std::endl;
	o << "viewOrigin:       " << p.splits[1].viewOrigin << std::endl;
	o << "viewAngles:       " << p.splits[1].viewAngles << std::endl;
	o << "localViewAngles:  " << p.splits[1].localViewAngles << std::endl;
	o << "viewOrigin:       " << p.splits[1].viewOrigin2 << std::endl;
	o << "viewAngles:       " << p.splits[1].viewAngles2 << std::endl;
	o << "localViewAngles:  " << p.splits[1].localViewAngles2 << std::endl;
	o << "}\n";

	return (o);
}