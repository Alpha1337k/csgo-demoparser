#include <demo.hpp>

Player::Player(std::string &data)
{
	memcpy(this, &data[0], sizeof(md));

	md.version = bswap_64(md.version);
	md.xUid = bswap_64(md.xUid);
	md.userId = bswap_32(md.userId);
	md.friendsId = bswap_32(md.friendsId);

}
