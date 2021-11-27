#include <demo.hpp>

Player::Player(std::string &data)
{
	memcpy(this, &data[0], sizeof(*this));

	version = bswap_64(version);
	xUid = bswap_64(xUid);	
	userId = bswap_32(userId);
	friendsId = bswap_32(friendsId);

}
