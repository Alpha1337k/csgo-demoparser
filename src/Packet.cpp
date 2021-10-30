#include <demo.hpp>

Packet::Packet(FILE *f)
{
	fread(this, 1, sizeof(f), f);
}

std::ostream &operator<<(std::ostream &o, const Packet &p)
{
	o << "Packet {\n";
	o << "viewOrigin:       " << p.viewOrigin << std::endl;
	o << "viewAngles:       " << p.viewAngles << std::endl;
	o << "localViewAngles:  " << p.localViewAngles << std::endl;
	o << "viewOrigin:       " << p.viewOrigin2 << std::endl;
	o << "viewAngles:       " << p.viewAngles2 << std::endl;
	o << "localViewAngles:  " << p.localViewAngles2 << std::endl;
	o << "}\n";

	return (o);
}