#ifndef DEMFILE_HPP
#define DEMFILE_HPP

#include <vector>
#include <stdio.h>
#include <exception>
#include "../out/packetmessages.pb.h"

#define MessageVector std::vector<std::pair<PacketTypes, void *>>

enum PacketTypes
{
	// it's a startup message, process as fast as possible
	dem_signon	= 1,
	// it's a normal network packet that we stored off
	dem_packet,
	// sync client clock to demo tick
	dem_synctick,
	// console command
	dem_consolecmd,
	// user input command
	dem_usercmd,
	// network data tables
	dem_datatables,
	// end of time.
	dem_stop,
	// a blob of binary data understood by a callback function
	dem_customdata,

	dem_stringtables,

	// Last command
	dem_lastcmd		= dem_stringtables,

    MSG_SERVER_INFO = 8,
	
    MSG_DATA_TABLE = 9,
    MSG_CREATE_STRING_TABLE = 12,
    MSG_UPDATE_STRING_TABLE = 13,
    MSG_USER_MESSAGE = 23,
    MSG_GAME_EVENT = 25,
    MSG_PACKET_ENTITIES = 26,
    MSG_GAME_EVENTS_LIST = 30
};

struct Vector
{
	float x, y, z;
	void Init( void )
	{
		x = y = z = 0.0f;
	}
	void Init( float _x, float _y, float _z )
	{
		x = _x;
		y = _y;
		z = _z;
	}
};
std::ostream &operator<<(std::ostream &o, const Vector &d);

struct DemHeader
{
	char 	headerDef[8];
	int		demoProtocol;
	unsigned int		networkProtocol;
	char	serverName[260];
	char	clientName[260];
	char	mapName[260];
	char	gameDir[260];
	float	demoLength;
	int		ticks;
	int		frames;
	int		signOnLength;

};
std::ostream &operator<<(std::ostream &o, const DemHeader &d);

struct Packet
{
	struct Split_t
	{
		int			flags;

			// original origin/viewangles
		Vector		viewOrigin;
		Vector		viewAngles;
		Vector		localViewAngles;

			// Resampled origin/viewangles
		Vector		viewOrigin2;
		Vector		viewAngles2;
		Vector		localViewAngles2;
	};
	Split_t splits[2];
	MessageVector msg;
	Packet(FILE *f);
	Packet() {}
};
std::ostream &operator<<(std::ostream &o, const Packet &d);

struct Frame
{
	unsigned char	cmd;
	int				tick;
	char			playerslot;

	Packet			pckt;
	Frame(FILE *f, bool &finished);
};
std::ostream &operator<<(std::ostream &o, const Frame &d);

struct DataTable
{

	DataTable(FILE *f);
};

class DemoFile
{
private:
	DemHeader header;
	ServerInfo *info;
	std::string	signOnData;
	std::vector<Frame> frames;
	std::vector<GameEventList_descriptor_t> gEvents;
	std::vector<CreateStringTable> sTables;

	void handleGameEventList(GameEventList &ge);
	void handleGameEvent(GameEvent &ge);
	void handleServerInfo(ServerInfo &si);
	void handleCreateStringTable(CreateStringTable &si);
	void handleUpdateStringTable(UpdateStringTable &si);
	void handlePacketEntities(PacketEntities &e);
public:
	DemoFile(FILE *f);
	~DemoFile();
	void	create_metrics();
};

struct GameEventListParsed
{

	GameEventListParsed(GameEventList &l);
};

struct GameEventParsed
{

	GameEventParsed(GameEvent &l);
};

std::string	readVarString(FILE *f, size_t *iter);
unsigned int readVarInt(FILE *f, size_t *iter);
bool	readVarBool(FILE *f, size_t *iter);
MessageVector getProtoMesssages(FILE *f, int size);
#endif