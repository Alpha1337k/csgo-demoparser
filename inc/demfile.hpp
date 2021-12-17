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
#include <math.h>
#include <chrono>
#include <unordered_map>

// #define sr.readBits(x) readStringBits(data, x, i, bitsAvailable)
// #define StreamReader &sr const std::string &data, int &i, char &bitsAvailable
// #define sr data, i, bitsAvailable

#define printIfAllowed(query, toPrint)	\
	{									\
		if (startupParameters[#query])	\
			toPrint;					\
	}

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

enum DecodedTypes
{
	decoded_int,
	decoded_float,
	decoded_Vector,
	decoded_Vector2,
	decoded_string
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

class FileReader
{
	private:
		std::string data;
		size_t		idx;
	public:
		FileReader(std::string path);
		FileReader();
		~FileReader();
		int				load(std::string path);
		int				loadFd(int fd);
		size_t			read(void *buffer, size_t size);
		bool			isEof();
		char			*getCurrentChar();
		void			ForceIncrement(size_t len);
		int				readInt();
		std::string		readString();
		size_t			getOffset();
};

class	StreamReader
{
	public:
		const std::string &data;
		int			idx;
		char		bitsAvailable;
		unsigned int buffer;
		void		loadBuffer();
	public:
		StreamReader(const std::string &d);
		int			readBits(int len);
		int		readBit();
		int			readStreamInt();
		bool		isEof();
};

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
	Packet(FileReader &f);
	Packet();
};
std::ostream &operator<<(std::ostream &o, const Packet &d);

struct Frame
{
	unsigned char	cmd;
	int				tick;
	char			playerslot;

	Packet			pckt;
	Frame(FileReader &f, bool &finished);
	Frame(const Frame &f);
	Frame &operator=(const Frame &f);
};
std::ostream &operator<<(std::ostream &o, const Frame &d);

struct PropW
{
	SendTable_sendprop_t 	prop;
	std::string				path;
	SendTable_sendprop_t	targetElem;

	PropW(const SendTable_sendprop_t &p, std::string pth, const SendTable_sendprop_t *target = 0)
	{
		path = pth;
		prop = p;
		if (target)
			targetElem = *target;
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


		bool isPropExcluded(const DataTable &dt, const SendTable &st, const SendTable_sendprop_t &p);
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

	DataTable(FileReader &f);
	DataTable();
	DataTable &operator=(const DataTable &d);
	void	shallowSwap(DataTable &d);

	SendTable	*findSendTable(std::string name);
};
std::ostream &operator<<(std::ostream &o, const DataTable::ServiceClass &p);

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

class GameEntities
{
	public:
		struct Property
		{
			int type;
			void	*data;
			Property		&operator=(const Property &s);
			Property(): data(0) {}
			~Property();
		};
		struct Entity
		{
			DataTable::ServiceClass *parentService;
			std::unordered_map<std::string, Property>	properties;

			Entity			&operator=(const Entity &s);
			void UpdateEntity(Entity &s);
		};
	private:
		std::unordered_map<int, Entity>					props;
	public:
		GameEntities();
		void	parse(PacketEntities &pe, DataTable &dt, DemoFile &df);
		void	executeChanges(class DemoFile &df);
};
std::ostream &operator<<(std::ostream &o, const GameEntities::Entity &e);


struct Player {
	struct Metadata
	{
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
	};

	Metadata	md;
	GameEntities::Entity *packetRef;

	const GameEntities::Property *getProperty(std::string name) const;
	Player(std::string &data);
	Player() {}
	Player(const Player &p) {*this = p;}
	Player &operator=(const Player &p);
};
std::ostream &operator<<(std::ostream &o, const Player &p);

class DemoFile
{
private:
	DemHeader header;
	ServerInfo *info;
	std::string	signOnData;
	std::vector<Frame> frames;
	std::vector<GameEventList_descriptor_t> gEvents;
	std::vector<ParsedStringTable>			parsedTables;
	std::unordered_map<int, Player>					players;
	DataTable							dataTable;
	GameEntities						entities;
	std::vector<void (*)(void *)>		eventHooks;
	size_t								tick;

	void handleGameEventList(GameEventList &ge);
	void handleGameEvent(GameEvent &ge);
	void handleServerInfo(ServerInfo &si);
	void handleCreateStringTable(CreateStringTable &si);
	void handleUpdateStringTable(UpdateStringTable &si);
	void handlePacketEntities(PacketEntities &e);
	void handleUserMessage(UserMessage &e);
	void handleDataTable(DataTable &dt);
public:
	size_t totalparse;
	size_t totalset;

	DemoFile(FileReader &f);
	~DemoFile();
	void	create_metrics();
	
	void	addPlayer(Player &p, int idx);
	const	std::unordered_map<int, Player> &getPlayers();
	Player &getPlayer(size_t idx);

	const GameEventList_descriptor_t &getGameEvent(size_t idx);

	const size_t	getCurrentTick();

	void	addEventHook(int type, void (*f)(void *data));
	void	removeEventHook(int type);
};

void getProtoMesssages(FileReader &f, int size, MessageVector &msg);
int readVarInt(char *ar, size_t *iter);
int		readStringBits(const std::string &str, int count, int &i, char &bitsAvailable);
int		readStringVarInt(const std::string &str, int &i, char &bitsAvailable);

/* decoders */
int decodeint(StreamReader &sr, const SendTable_sendprop_t &prop);
float decodefloat(StreamReader &sr, const SendTable_sendprop_t &prop);
Vector decodeVector(StreamReader &sr, const SendTable_sendprop_t &prop);
Vector2 decodeVector2(StreamReader &sr, const SendTable_sendprop_t &prop);
std::string decodestring(StreamReader &sr, const SendTable_sendprop_t &prop);



#endif