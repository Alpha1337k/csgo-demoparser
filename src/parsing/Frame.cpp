#include <demo.hpp>

Packet handlePacket(FILE *f)
{
	Packet p(f);

	// std::cout << p << std::endl;

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

	assert( cmd >= 1 && cmd <= dem_lastcmd );

	switch (cmd)
	{
		case dem_signon:
		case dem_packet:
			pckt = handlePacket(f);
			break;
		case dem_stop:
			finished = true;
			break;
		case dem_consolecmd:
			assert(0);
			break;
		case dem_datatables:
		{
			DataTable *d = new DataTable(f);
			pckt.msg.push_back(std::make_pair(svc_DataTable, d));
			break;
		}
		case dem_stringtables:
			assert(0);
			break;
		case dem_usercmd:
			assert(0);
			break;
		case dem_synctick:
		default:
		{
			break;
		}
	}
}
