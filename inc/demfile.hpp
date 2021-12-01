#ifndef DEMFILE_HPP
#define DEMFILE_HPP

#include <vector>
#include <stdio.h>
#include <exception>
#include <netmessages.pb.h>
#include <csgomsg.pb.h>
#include <stdlib.h>
#include <bswap.h>
#include <sstream>
#include <algorithm>

#define readBits(x) readStringBits(data, x, i, bitsAvailable)
#define standardParameters const std::string &data, int &i, char &bitsAvailable
#define standardIParameters data, i, bitsAvailable

#define ErrorReturnMessage(msg) \
	{ std::cerr << msg << std::endl; return; }
#define ReturnMessage(msg) \
	{ std::cout << msg << std::endl; return; }

#define MessageVector std::vector<std::pair<int, void *>>

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
};

enum PreParsedPackages
{
	svc_DataTable = -1
};

struct Vector
{
	float x, y, z;
	void Init( void );
	void Init( float _x, float _y, float _z );
};
std::ostream &operator<<(std::ostream &o, const Vector &d);

struct Vector2
{
	float x, y;
	void Init( void );
	void Init( float _x, float _y, float _z );
};
std::ostream &operator<<(std::ostream &o, const Vector2 &d);

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
	Packet();
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

struct PropW
{
	SendTable_sendprop_t 	prop;
	std::string				path;

	PropW(const SendTable_sendprop_t &p, std::string pth)
	{
		path = pth;
		prop = p;

	}
};
std::ostream &operator<<(std::ostream &o, const PropW &p);
std::ostream &operator<<(std::ostream &o, const SendTable_sendprop_t &p);

struct DataTable
{
	struct ServiceClass
	{
		short id;
		std::string name;
		std::string nameDataTable;
		SendTable	*dataTable;
		std::vector<PropW> props;


		bool isPropExcluded(DataTable &dt, const SendTable_sendprop_t &p);
		void flattenProps(DataTable &dt, SendTable *send = 0);
		void gatherExcludes(DataTable &dt, const SendTable &st);
		void gatherProps(DataTable &dt, SendTable &st, std::string path = "");
		void iterateProps(DataTable &dt, const SendTable &send, \
				std::vector<PropW> &store, std::string path = "");
		void sortProps();

		ServiceClass &operator=(const ServiceClass &cs);
	};

	std::vector<ServiceClass> services;
	std::vector<SendTable_sendprop_t> excludedProps;
	MessageVector msg;
	char	serviceClassBits;

	DataTable(FILE *f);
	DataTable();
	DataTable &operator=(const DataTable &d);

	SendTable	*findSendTable(std::string name);
};

struct Player {
	long	version;
	long	xUid;
	char	userName[128];
	int		userId;
	char	gUid[33];
	int		friendsId;
	char	friendsName[128];
	bool	isFake;
	bool	isHltv;

	int		customFiles[4];
	unsigned char	filesDownloaded;

	Player(std::string &data);
};
std::ostream &operator<<(std::ostream &o, const Player &p);


class ParsedStringTable
{
private:
	void Update(const std::string &data, bool isUserInfo, int num_changed_entries, class DemoFile &df);
public:
	CreateStringTable &origin;
	ParsedStringTable(CreateStringTable &st, class DemoFile &df);
	void	Update(CreateStringTable &st, class DemoFile &df);
	void	Update(UpdateStringTable &ut, class DemoFile &df, bool isUserInfo = false);
	~ParsedStringTable();
};

class ParsedPacketEntities
{
	private:
	
	public:
		ParsedPacketEntities(PacketEntities &pe, const DataTable &dt);
};


class DemoFile
{
private:
	DemHeader header;
	ServerInfo *info;
	std::string	signOnData;
	std::vector<Frame> frames;
	std::vector<GameEventList_descriptor_t> gEvents;
	std::vector<ParsedStringTable>			parsedTables;
	std::vector<Player>					players;
	std::vector<DataTable::ServiceClass>	entities;
	DataTable							dataTable;

	void handleGameEventList(GameEventList &ge);
	void handleGameEvent(GameEvent &ge);
	void handleServerInfo(ServerInfo &si);
	void handleCreateStringTable(CreateStringTable &si);
	void handleUpdateStringTable(UpdateStringTable &si);
	void handlePacketEntities(PacketEntities &e);
	void handleUserMessage(UserMessage &e);
	void handleDataTable(DataTable &dt);
public:
	DemoFile(FILE *f);
	~DemoFile();
	void AddPlayer(Player &p);
	void	create_metrics();

};

std::string	readVarString(FILE *f, size_t *iter);
std::string	readVarString(const std::string &str, int &iter);
int readVarInt(FILE *f, size_t *iter);
bool	readVarBool(FILE *f, size_t *iter);
MessageVector getProtoMesssages(FILE *f, int size);
int readVarInt(char *ar, size_t *iter);
int		readStringBits(const std::string &str, int count, int &i, char &bitsAvailable);
int		readStringVarInt(const std::string &str, int &i, char &bitsAvailable);
std::string replaceAll(std::string str, const std::string from, const std::string to);

/* decoders */
int decodeint(standardParameters, const SendTable_sendprop_t &prop);
float decodefloat(standardParameters, const SendTable_sendprop_t &prop);
Vector decodeVector(standardParameters, const SendTable_sendprop_t &prop);
Vector2 decodeVector2(standardParameters, const SendTable_sendprop_t &prop);
std::string decodestring(standardParameters, const SendTable_sendprop_t &prop);



#endif