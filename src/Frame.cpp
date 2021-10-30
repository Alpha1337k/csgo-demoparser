#include <demo.hpp>

Packet handlePacket()
{
	Packet p(NULL);


	return p;
}

Frame::Frame(FILE *f, bool &finished)
{
	size_t size;
	size = fread(&cmd, sizeof(cmd), 1, f);
	size = fread(&tick, sizeof(tick), 1, f);
	size = fread(&playerslot, sizeof(playerslot), 1, f);
	switch (cmd)
	{
		case dem_signon:
		case dem_packet:
			handlePacket();
		case dem_stop:
			finished = false;
			break;
		case dem_consolecmd:
			break;
		case dem_datatables:
			break;
		case dem_stringtables:
			break;
		case dem_usercmd:
			break;
		default:
			break;
	}
}

std::ostream &operator<<(std::ostream &o, const Frame &f)
{
	o << "Frame {\n";
	o << "cmd:        " << (int)f.cmd << std::endl;
	o << "tick:       " << f.tick << std::endl;
	o << "playerslot: " << (int)f.playerslot << std::endl;
	o << "}\n";

	return (o);
}