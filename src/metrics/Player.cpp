#include <demo.hpp>

Player::Player(std::string &data)
{
	memcpy(this, &data[0], sizeof(*this));

	version = bswap_64(version);
	xUid = bswap_64(xUid);	
	userId = bswap_32(userId);
	friendsId = bswap_32(friendsId);

}

std::ostream &operator<<(std::ostream &o, const Player &p)
{
	o << "Player: {" << std::endl;
	o << "version: " << p.version << std::endl;
	o << "xUid: " << p.xUid << std::endl;
	o << "userName: " << p.userName << std::endl;
	o << "userId: " << p.userId << std::endl;
	o << "gUid: " << p.gUid << std::endl;
	o << "friendsId: " << p.friendsId << std::endl;
	o << "friendsName: " << p.friendsName << std::endl;
	o << "isFake: " << p.isFake << std::endl;
	o << "isHltv: " << p.isHltv << std::endl;
	o << "customFiles: " << p.friendsName << std::endl;
	o << "filesDownloaded: " << (unsigned int)p.filesDownloaded << std::endl;
	o << "}" << std::endl;

	return o;
}