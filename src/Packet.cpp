#include <demo.hpp>

Packet::Packet(FILE *f)
{
	//viewOrigin.Init();
	//viewOrigin2.Init();
	//localViewAngles.Init();
	//localViewAngles2.Init();
	//viewAngles.Init();
	//viewAngles2.Init();

	fread(this, 1, sizeof(*this), f);

	int sequenceIn = 0, sequenceOut = 0;
	size_t size = 0;
	unsigned int chunckSize = 0;
	size = fread(&sequenceIn, sizeof(int), 1, f);
	size = fread(&sequenceOut, sizeof(int), 1, f);
	std::cout << "sequence: " << sequenceIn << ", " << sequenceOut << std::endl;
	fread(&chunckSize, sizeof(int), 1, f);
	std::cout << "size: " << chunckSize << std::endl;
	size_t iter = 0;
	while (iter < chunckSize && iter < 100)
	{
		char messagetype = 0;
		char tmp[3];
		int length = 0;
		iter += fread(&messagetype, 1, sizeof(messagetype), f);
		iter += fread(&tmp, 1, sizeof(tmp), f);

		iter += fread(&length, 1, sizeof(length), f);
		
		std::cout << "Iter: " << iter << " messagetype:" << (int)messagetype << std::endl;
		std::cout << "Length: " << length << std::endl;

		break;
	}
		
	 
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