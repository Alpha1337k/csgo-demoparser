#include <string>
#include <string.h>
#include <iostream>
#include "../out/packetmessages.pb.h"

struct ServerInfo
{
	CSVCMsg_ServerInfo svi;
	
	ServerInfo(FILE *f, unsigned int len);
};
std::ostream &operator<<(std::ostream &o, const ServerInfo &i);
