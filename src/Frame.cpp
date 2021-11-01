#include <demo.hpp>

Packet handlePacket(FILE *f)
{
	Packet p(f);

	std::cout << p << std::endl;

	return p;
}

Frame::Frame(FILE *f, bool &finished)
{
	cmd = 0;
	tick = 0;
	playerslot = 0;
	fread(&cmd, 1, sizeof(cmd), f);
	fread(&tick, 1, sizeof(tick), f);
	fread(&playerslot, 1, sizeof(playerslot), f);

	std::cout << "cmd: " << (int)cmd << std::endl;
	assert( cmd >= 1 && cmd <= dem_lastcmd );

	switch (cmd)
	{
		case dem_signon:
		case dem_packet:
			handlePacket(f);
			break;
		case dem_stop:
			finished = true;
			break;
		case dem_consolecmd:
			std::cout << "UnIMPLEMENTED!!! cons" << std::endl;
			exit(0);
			break;
		case dem_datatables:
		{
			DataTable d = DataTable(f);
			(void)d;
			break;
		}
		case dem_stringtables:
			std::cout << "UnIMPLEMENTED!!! strig" << std::endl;
			exit(0);
			break;
		case dem_usercmd:
			std::cout << "UnIMPLEMENTED!!! usr" << std::endl;
			exit(0);
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