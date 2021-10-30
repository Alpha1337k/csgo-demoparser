#include <demo.hpp>
#include <packetstructs.hpp>

ServerInfo::ServerInfo(FILE *f, unsigned int len)
{
	std::string tmp;
	tmp.resize(len);
	fread(&tmp[0], 1, len, f);

	if (svi.ParseFromString(tmp) == false)
		std::cout << "Wow we cant parse this, why?" << std::endl;
}

std::ostream &operator<<(std::ostream &o, const ServerInfo &i)
{
	o << "DemHeader {\n";
	o << i.svi.DebugString() << std::endl;
	o << "}\n";

	return (o);
}