#ifndef DEMFILE_HPP
#define DEMFILE_HPP

#include <vector>
#include <deque>
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
#include <variant>
#include <FileReader.hpp>
#include <StreamReader.hpp>
#include <functional>

#define printIfAllowed(query, toPrint)	\
	{									\
		if (startupParameters[#query])	\
			toPrint;					\
	}

#define ErrorReturnMessage(msg) \
	{ std::cerr << msg << std::endl; return; }
#define ReturnMessage(msg) \
	{ std::cout << msg << std::endl; return; }

// maybe a deque is better, but a simple bench showed that vector is stil king
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
	decoded_string,
	decoded_array
};


enum PreParsedPackages
{
	svc_DataTable = -1,
	svc_UpdateEntity = 49,
	svc_CreateEntity = 48,
	svc_DeleteEntity = 47,
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

struct Frame
{
	unsigned char	cmd;
	int				tick;
	char			playerslot;
	int 			index;

	Frame(FileReader &f, bool &finished, class DemoFile &d);
	Frame(const Frame &f);
	Frame &operator=(const Frame &f);
};
std::ostream &operator<<(std::ostream &o, const Frame &d);

struct PropW
{
	SendTable_sendprop_t 	*prop;
	std::string				path;
	int						targetElem;

	PropW(const SendTable_sendprop_t &p, std::string &pth, int target = -1)
	{
		prop = (SendTable_sendprop_t *)&p;
		path = pth;
		targetElem = target;
	}
	PropW(const PropW &p) {*this = p;}
	PropW &operator=(const PropW &p)
	{
		prop = p.prop;
		path = p.path;
		targetElem = p.targetElem;

		return *this;
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
		std::vector<PropW> arProps;


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
	std::vector<SendTable> msg;
	char	serviceClassBits;

	DataTable(FileReader &f, class DemoFile &df);
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
			int flags;
			int num_bits;
			float low_value;
			float high_value;

			std::variant<int, float, std::string, Vector, Vector2, std::vector<Property> > data;
			Property(const Property &s) {*this = s;}
			Property		&operator=(const Property &s);
			Property() {}
			~Property();
		};
		struct Entity
		{
			DataTable::ServiceClass *parentService;
			std::vector<std::pair<const std::string *, Property> >	properties;

			const GameEntities::Property *getProperty(std::string name) const;
			Entity(): parentService(0) {}
			Entity(const Entity &s): parentService(0) {*this = s;}
			Entity			&operator=(const Entity &s);
			void UpdateEntity(Entity &s);
		};
	private:
		std::deque<Entity>							props;
		std::unordered_multimap<std::string, int>	indexes;

	public:
		inline	std::pair	<std::unordered_multimap<std::string, int>::iterator, \
							std::unordered_multimap<std::string, int>::iterator> \
							getEntitiesByName(std::string name) {return indexes.equal_range(name);}
		inline	const std::unordered_multimap<std::string, int>	getEntities() {return indexes;}
		inline	const Entity *getEntity(int id)
		{
			if (id > props.size())
				return 0;
			return &props[id];
		}

		GameEntities();
		void	parse(PacketEntities &pe, DataTable &dt, DemoFile &df);
		void	executeChanges(class DemoFile &df);
};
std::ostream &operator<<(std::ostream &o, const GameEntities::Property &p);
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

	const GameEntities::Property *getProperty(std::string name);
	Player(std::string &data);
	Player(): packetRef(0) {}
	Player(const Player &p): packetRef(0) {*this = p;}
	Player &operator=(const Player &p);
};
std::ostream &operator<<(std::ostream &o, const Player &p);

class DemoFile
{
private:
	DemHeader header;
	ServerInfo *info;
	std::vector<GameEventList_descriptor_t> gEvents;
	std::vector<ParsedStringTable>			parsedTables;
	std::unordered_map<int, Player>			players;
	DataTable							dataTable;
	GameEntities						entities;
	std::vector<std::function<void (void *)>>		eventHooks;

	void handleGameEventList(GameEventList &ge);
	void handleGameEvent(GameEvent &ge);
	void handleServerInfo(ServerInfo &si);
	void handleCreateStringTable(CreateStringTable &si);
	void handleUpdateStringTable(UpdateStringTable &si);
	void handlePacketEntities(PacketEntities &e);
	void handleUserMessage(UserMessage &e);
	void handleSendTable(SendTable &st);
public:
	size_t totalparse;
	DataTable	&getDataTable() { return dataTable; }
	void handleDataTable();

	DemoFile();
	~DemoFile();

	void	start_parse(FileReader &f);
	void	handle_packet(int type, void *data);

	void	addPlayer(Player &p, int idx);
	std::unordered_map<int, Player> &getPlayers();
	inline	std::pair	<std::unordered_multimap<std::string, int>::iterator, \
						std::unordered_multimap<std::string, int>::iterator> \
						getEntitiesByName(std::string name) {return entities.getEntitiesByName(name);}
	inline	const std::unordered_multimap<std::string, int>	getEntities() {return entities.getEntities();}
	inline	const GameEntities::Entity *getEntity(int id) {return entities.getEntity(id);}

	Player &getPlayer(size_t idx);

	const GameEventList_descriptor_t &getGameEvent(size_t idx);

	const size_t	getCurrentTick();

	void	addEventHook(int type, std::function<void (void *)> &f);
	void	removeEventHook(int type);
	inline void emitEvent(int type, void *val) {
		if (eventHooks[type]) 
			eventHooks[type](val);
	}
};

void getProtoMesssages(FileReader &f, int size, DemoFile &d);
int readVarInt(char *ar, size_t *iter);
int		readStringBits(const std::string &str, int count, int &i, char &bitsAvailable);
int		readStringVarInt(const std::string &str, int &i, char &bitsAvailable);

/* decoders */
int decodeint(StreamReader &sr, const GameEntities::Property &prop);
float decodefloat(StreamReader &sr, const GameEntities::Property &prop);
Vector decodeVector(StreamReader &sr, const GameEntities::Property &prop);
Vector2 decodeVector2(StreamReader &sr, const GameEntities::Property &prop);
std::string decodestring(StreamReader &sr, const GameEntities::Property &prop);
void printdecTime();


#endif