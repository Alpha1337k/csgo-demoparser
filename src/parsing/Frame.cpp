#include <demo.hpp>

Frame::Frame(FileReader &f, bool &finished)
{
	cmd = 0;
	tick = 0;
	playerslot = 0;
	f.read(&cmd, sizeof(cmd));
	f.read(&tick, sizeof(tick));
	f.read(&playerslot, sizeof(playerslot));

	assert( cmd >= 1 && cmd <= dem_lastcmd );

	switch (cmd)
	{
		case dem_signon:
		case dem_packet:
			pckt.Load(f);
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

Frame::Frame(const Frame &f) {*this = f;}

Frame	&Frame::operator=(const Frame &f)
{
	cmd = f.cmd;
	pckt = f.pckt;
	tick = f.tick;
	playerslot = f.playerslot;


	return *this;
}